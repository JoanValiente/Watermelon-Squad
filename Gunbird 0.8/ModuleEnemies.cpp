#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleBoss.h"
#include "ModulePlayer2.h"
#include "ModuleEnemies.h"
#include "ModuleParticles.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "Enemy.h"
#include "Enemy_Balloon.h"
#include "Enemy_FlyingMachine.h"
#include "Enemy_FlyingMachine2.h"
#include "Enemy_FlyingMachine3.h"
#include "Enemy_FlyingMachine4.h"
#include "Ship_Corpse.h"
#include "Enemy_Bomb.h"
#include "Enemy_Bomb2.h"
#include "Enemy_VerticalBomb.h"
#include "Enemy_Tower.h"
#include "Enemy_SurfingTurret.h"
#include "Enemy_SurfingTurret2.h"
#include "Enemy_VerticalSurfingTurret.h"
#include "Enemy_UpSurfingTurret.h"
#include "Enemy_StaticTurret.h"
#include "Enemy_StaticTurret2.h"
#include "Enemy_StaticTurret3.h"
#include "Enemy_StaticTurret4.h"
#include "Enemy_BlueTurret.h"
#include "Enemy_BlueRobot.h"
#include "Enemy_BlueRobot2.h"
#include "Enemy_RedMachine1.h"
#include "Enemy_RedMachine2.h"
#include "Enemy_ShipTurret.h"
#include "Enemy_BigShipStatic.h"
#include "Enemy_BigShipMove.h"
#include "Enemy_BigShipMove2.h"
#include "Enemy_ShipHorizontalCanon.h"
#include "Enemy_ShipVerticalCanon.h"
#include "Enemy_ShipVerticalCanon2.h"
#include "Enemy_ShipVerticalCanon3.h"
#include "Enemy_ShipVerticalCanonLittle.h"
#include "Enemy_ShipBigMiddleTurret.h"
#include "Enemy_ShipBackTurret.h"
#include "Enemy_BirdBody.h"
#include "Enemy_LeftBirdWing.h"
#include "Enemy_RightBirdWing.h"
#include "Enemy_Ship.h"
#include "ExtraBomb.h"
#include "PowerUp.h"


#define SPAWN_MARGIN 140

ModuleEnemies::ModuleEnemies()
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
		enemies[i] = nullptr;
}

// Destructor
ModuleEnemies::~ModuleEnemies()
{
}

bool ModuleEnemies::Start()
{
	// Create a prototype for each enemy available so we can copy them around
	sprites = App->textures->Load("assets/enemies.png");
	sprites2 = App->textures->Load("assets/particles.png");
	
	audio_explosion = App->audio->LoadFX("assets/Audio/explosion.wav");
	audio_explosion2 = App->audio->LoadFX("assets/Audio/explosion2.wav");
	audio_explosion3 = App->audio->LoadFX("assets/Audio/explosion3.wav");
	audio_explosion4 = App->audio->LoadFX("assets/Audio/explosion4.wav");
	audio_explosion5 = App->audio->LoadFX("assets/Audio/explosion5.wav");


	return true;
}

update_status ModuleEnemies::PreUpdate()
{
	// check camera position to decide what to spawn
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type != ENEMY_TYPES::NO_TYPE)
		{

			if (queue[i].y * SCREEN_SIZE < App->render->camera.y + (App->render->camera.h * SCREEN_SIZE) + SPAWN_MARGIN)
			{
				SpawnEnemy(queue[i]);
				queue[i].type = ENEMY_TYPES::NO_TYPE;
				LOG("Spawning enemy at %d", queue[i].y * SCREEN_SIZE);
			}
		}
	}


	return UPDATE_CONTINUE;
}

// Called before render is available
update_status ModuleEnemies::Update()
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			enemies[i]->Move();
		}
	}

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			enemies[i]->Shoot();
		}
	}

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			enemies[i]->Draw(sprites);
		}
	}

	return UPDATE_CONTINUE;
}

update_status ModuleEnemies::PostUpdate()
{
	// check camera position to decide what to spawn
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			
			int si1 = enemies[i]->position.y *(-SCREEN_SIZE);
			int si2 = (App->render->camera.y - (SCREEN_HEIGHT*SCREEN_SIZE));
				if (si1 < si2)
				{

					LOG("DeSpawning enemy at %d", enemies[i]->position.y * SCREEN_SIZE);
					delete enemies[i];
					enemies[i] = nullptr;
					}
		}
		if (enemies[i] != nullptr && enemies[i]->enemy == 17)
		{

			int si1 = enemies[i]->position.y *(-SCREEN_SIZE);
			int si2 = (App->render->camera.y - (SCREEN_HEIGHT*SCREEN_SIZE + SPAWN_MARGIN));
			if (si1 < si2)
			{

				LOG("DeSpawning enemy at %d", enemies[i]->position.y * SCREEN_SIZE);
				delete enemies[i];
				enemies[i] = nullptr;
			}
		}
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEnemies::CleanUp()
{
	LOG("Freeing all enemies");

	App->textures->Unload(sprites);
	App->textures->Unload(sprites2);

	App->audio->UnloadFX(audio_explosion);
	App->audio->UnloadFX(audio_explosion2);
	App->audio->UnloadFX(audio_explosion3);
	App->audio->UnloadFX(audio_explosion4);
	App->audio->UnloadFX(audio_explosion5);

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
	}

	return true;
}

bool ModuleEnemies::AddEnemy(ENEMY_TYPES type, int x, int y)
{
	bool ret = false;

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type == ENEMY_TYPES::NO_TYPE)
		{
			queue[i].type = type;
			queue[i].x = x;
			queue[i].y = y;
			ret = true;
			break;
		}
	}

	return ret;
}

void ModuleEnemies::SpawnEnemy(const EnemyInfo& info)
{
	// find room for the new enemy
	uint i = 0;
	for (; enemies[i] != nullptr && i < MAX_ENEMIES; ++i);

	if (i != MAX_ENEMIES)
	{
		switch (info.type)
		{

		case ENEMY_TYPES::BALLOON:
			enemies[i] = new Enemy_Balloon(info.x, info.y);
			break;

		case ENEMY_TYPES::FLYINGMACHINE:
			enemies[i] = new Enemy_FlyingMachine(info.x, info.y);
			break;

		case ENEMY_TYPES::FLYINGMACHINE2:
			enemies[i] = new Enemy_FlyingMachine2(info.x, info.y);
			break;

		case ENEMY_TYPES::FLYINGMACHINE3:
			enemies[i] = new Enemy_FlyingMachine3(info.x, info.y);
			break;

		case ENEMY_TYPES::FLYINGMACHINE4:
			enemies[i] = new Enemy_FlyingMachine4(info.x, info.y);
			break;

		case ENEMY_TYPES::BOMB:
			enemies[i] = new Enemy_Bomb(info.x, info.y);
			break;

		case ENEMY_TYPES::BOMB2:
			enemies[i] = new Enemy_Bomb2(info.x, info.y);
			break;

		case ENEMY_TYPES::VERTICALBOMB:
			enemies[i] = new Enemy_VerticalBomb(info.x, info.y);
			break;

		case ENEMY_TYPES::POWER_UP:
			enemies[i] = new PowerUp(info.x, info.y);
			break;

		case ENEMY_TYPES::TOWER:
			enemies[i] = new  Enemy_Tower(info.x, info.y);
			break;

		case ENEMY_TYPES::SURFINGTURRET:
			enemies[i] = new  Enemy_SurfingTurret(info.x, info.y);
			break;

		case ENEMY_TYPES::SURFINGTURRET2:
			enemies[i] = new  Enemy_SurfingTurret2(info.x, info.y);
			break;

		case ENEMY_TYPES::VERTICALSURFINGTURRET:
			enemies[i] = new  Enemy_VerticalSurfingTurret(info.x, info.y);
			break;


		case ENEMY_TYPES::UPSURFINGTURRET:
			enemies[i] = new  Enemy_UpSurfingTurret(info.x, info.y);
			break;

		case ENEMY_TYPES::STATICTURRET:
			enemies[i] = new  Enemy_StaticTurret(info.x, info.y);
			break;

		case ENEMY_TYPES::STATICTURRET2:
			enemies[i] = new  Enemy_StaticTurret2(info.x, info.y);
			break;

		case ENEMY_TYPES::STATICTURRET3:
			enemies[i] = new  Enemy_StaticTurret3(info.x, info.y);
			break;

		case ENEMY_TYPES::STATICTURRET4:
			enemies[i] = new  Enemy_StaticTurret4(info.x, info.y);
			break;

		case ENEMY_TYPES::BLUETURRET:
			enemies[i] = new  Enemy_BlueTurret(info.x, info.y);
			break;

		case ENEMY_TYPES::BLUEROBOT:
			enemies[i] = new  Enemy_BlueRobot(info.x, info.y);
			break;

		case ENEMY_TYPES::BLUEROBOT2:
			enemies[i] = new  Enemy_BlueRobot2(info.x, info.y);
			break;

		case ENEMY_TYPES::REDMACHINE1:
			enemies[i] = new  Enemy_RedMachine1(info.x, info.y);
			break;

		case ENEMY_TYPES::REDMACHINE2:
			enemies[i] = new  Enemy_RedMachine2(info.x, info.y);
			break;

		case ENEMY_TYPES::SHIPTURRET:
			enemies[i] = new  Enemy_ShipTurret(info.x, info.y);
			break;

		case ENEMY_TYPES::BIGSHIPSTATIC:
			enemies[i] = new  Enemy_BigShipStatic(info.x, info.y);
			break;

		case ENEMY_TYPES::BIGSHIPMOVE:
			enemies[i] = new  Enemy_BigShipMove(info.x, info.y);
			break;

		case ENEMY_TYPES::BIGSHIPMOVE2:
			enemies[i] = new  Enemy_BigShipMove2(info.x, info.y);
			break;

		case ENEMY_TYPES::SHIPHORIZONTALCANON:
			enemies[i] = new  Enemy_ShipHorizontalCanon(info.x, info.y);
			break;

		case ENEMY_TYPES::SHIPVERTICALCANONLITTLE:
			enemies[i] = new  Enemy_ShipVerticalCanonLittle(info.x, info.y);
			break;

		case ENEMY_TYPES::SHIPVERTICALCANON:
			enemies[i] = new  Enemy_ShipVerticalCanon(info.x, info.y);
			break;

		case ENEMY_TYPES::SHIPVERTICALCANON2:
			enemies[i] = new  Enemy_ShipVerticalCanon2(info.x, info.y);
			break;

		case ENEMY_TYPES::SHIPVERTICALCANON3:
			enemies[i] = new  Enemy_ShipVerticalCanon3(info.x, info.y);
			break;
		case ENEMY_TYPES::SHIPBIGMIDDLETURRET:
			enemies[i] = new  Enemy_ShipBigMiddleTurret(info.x, info.y);
			break;

		case ENEMY_TYPES::SHIPBACKTURRET:
			enemies[i] = new  Enemy_ShipBackTurret(info.x, info.y);
			break;

		case ENEMY_TYPES::BIRDBODY:
			enemies[i] = new  Enemy_BirdBody(info.x, info.y);
			break;

		case ENEMY_TYPES::LEFTWINGBIRD:
			enemies[i] = new  Enemy_LeftBirdWing(info.x, info.y);
			break;
		case ENEMY_TYPES::RIGHTWINGBIRD:
			enemies[i] = new  Enemy_RightBirdWing(info.x, info.y);
			break;

		case ENEMY_TYPES::SHIP:
			enemies[i] = new  Enemy_Ship(info.x, info.y);
			break;

		case ENEMY_TYPES::EXTRABOMB:
			enemies[i] = new  ExtraBomb(info.x, info.y);
			break;


		case ENEMY_TYPES::SHIPCORPSE:
			enemies[i] = new  Ship_Corpse(info.x, info.y);
			break;
		}

	}
}

void ModuleEnemies::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr && enemies[i]->GetCollider() == c1)
		{
			if (c1->type == COLLIDER_POWER_UP) {
				delete enemies[i];
				enemies[i] = nullptr;
				break;
			}
			else if ((c1->type == COLLIDER_POWER_UP && c2->type == COLLIDER_ENEMY_SHOT) || (c1->type == COLLIDER_POWER_UP && c2->type == COLLIDER_ENEMY)) {}
			else if ((c2->type == COLLIDER_POWER_UP && c1->type == COLLIDER_ENEMY_SHOT) || (c2->type == COLLIDER_POWER_UP && c1->type == COLLIDER_ENEMY)) {}
			
			if (c1->type == COLLIDER_EXTRA_BOMB) {
				delete enemies[i];
				enemies[i] = nullptr;
				break;
			}
			
			if (c1->type == COLLIDER_ENEMY || c1->type == COLLIDER_FLYING_ENEMY) {

				if (App->player->powerUps == 0) { damage = 1; }
				if (App->player->powerUps == 1) { damage = 1.25f; }
				if (App->player->powerUps == 2) { damage = 1.50f; }
				if (App->player->powerUps == 3) { damage = 1.75f; }


				//BALLOON
				if (enemies[i]->enemy == 0) {
					LOG("BALLOON HIT POINTS %i", enemies[i]->hitpoints);
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_balloon, enemies[i]->position.x, enemies[i]->position.y, 0 , 0 , COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion);
						App->enemies->AddEnemy(POWER_UP, enemies[i]->position.x, enemies[i]->position.y);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//BIG SHIP
				else if(enemies[i]->enemy == 1) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_big_ship, enemies[i]->position.x+7, enemies[i]->position.y+16, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						AddEnemy(ENEMY_TYPES::SHIPCORPSE, enemies[i]->position.x, enemies[i]->position.y);;
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x -15, enemies[i]->position.y +10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x + 5, enemies[i]->position.y-10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion3);
						App->enemies->AddEnemy(EXTRABOMB, enemies[i]->position.x, enemies[i]->position.y);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//BLUE ROBOT
				else if (enemies[i]->enemy == 2) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_blue_robot, enemies[i]->position.x, enemies[i]->position.y-1, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y + 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x + 5, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x -15, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion);
						App->enemies->AddEnemy(POWER_UP, enemies[i]->position.x, enemies[i]->position.y);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//BLUE TURRET
				else if (enemies[i]->enemy == 3) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_turret, enemies[i]->position.x+6, enemies[i]->position.y+3, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->littleexplosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion4);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//BOMB
				else if (enemies[i]->enemy == 4) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_bomb, enemies[i]->position.x + 6, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->littleexplosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//FLYING MACHINES
				else if (enemies[i]->enemy == 5) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->littleexplosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion2);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//RED MACHINES
				else if (enemies[i]->enemy == 6) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_red_machine, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->littleexplosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//SURFING TURRETS
				else if (enemies[i]->enemy == 7 || enemies[i]->enemy == 17) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_turret, enemies[i]->position.x + 6, enemies[i]->position.y-5, 0, 0, COLLIDER_NONE);

					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->littleexplosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion4);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//TOWER
				else if (enemies[i]->enemy == 8) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_tower, enemies[i]->position.x, enemies[i]->position.y - 5, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y + 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x + 5, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion3);
						App->enemies->AddEnemy(POWER_UP, enemies[i]->position.x, enemies[i]->position.y);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

//	------------------------ BOSSS -------------------------
		// ----------SHIP------------
				
				//ShipBackTurret

				else if (enemies[i]->enemy == 9) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_shipbackturret, enemies[i]->position.x + 11, enemies[i]->position.y + 87, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x -100, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x +150, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x +75, enemies[i]->position.y +50, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x -10, enemies[i]->position.y+100, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->collision->shipturret2 = true;
						App->audio->PlayFX(audio_explosion);
						App->boss->spawnbird = true;
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}
		
		
		//BIG MIDDLE TOWER
				else if (enemies[i]->enemy == 10) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_shipbigmiddleturret, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->collision->shipturret1 = true;
						App->audio->PlayFX(audio_explosion);
						App->enemies->AddEnemy(POWER_UP, enemies[i]->position.x, enemies[i]->position.y);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//ShipHorizontalCanon
				else if (enemies[i]->enemy == 11) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_horizontalcanon, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y + 10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//ShipTurret

				else if (enemies[i]->enemy == 12) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_shipturret, enemies[i]->position.x+4, enemies[i]->position.y +5, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y + 10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}


				//ShipVerticalCanon

				else if (enemies[i]->enemy == 13) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						if (App->player->position.x < enemies[i]->position.x) {
							App->particles->AddParticle(App->particles->damage_shipverticalcanonleft, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						}

						else {
							App->particles->AddParticle(App->particles->damage_shipverticalcanonright, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						}
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y + 10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//BIRD BODY
				else if (enemies[i]->enemy == 14) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_bird_body, enemies[i]->position.x + 75, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y + 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y + 75, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x + 5, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y +40, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion);
						App->collision->deadbird = true;
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion5);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

				//BIRD LEFT WING
				else if (enemies[i]->enemy == 15) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
					
						App->particles->AddParticle(App->particles->damage_bird_lwing, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y + 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x + 5, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion4);
						App->enemies->AddEnemy(POWER_UP, enemies[i]->position.x, enemies[i]->position.y);
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}
				//BIRD RIGHT WING
				else if (enemies[i]->enemy == 16) {
					if (enemies[i]->hitpoints > 0) {
						if (c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_VALNUS_2_LASER) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 10);
						}
						else if (c2->type == COLLIDER_ASH_BOMB || c2->type == COLLIDER_ASH_BOMB_2) {
							enemies[i]->hitpoints = enemies[i]->hitpoints - (damage / 2);
						}
						else {
							enemies[i]->hitpoints = enemies[i]->hitpoints - damage;
						}
						App->particles->AddParticle(App->particles->damage_bird_rwing, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
					}
					else {
						if (c2->type == COLLIDER_PLAYER_2_SHOT || c2->type == COLLIDER_VALNUS_2_LASER || c2->type == COLLIDER_ASH_BOMB_2) {
							App->player2->score += enemies[i]->score;
						}
						if (c2->type == COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_VALNUS_LASER || c2->type == COLLIDER_ASH_BOMB) {
							App->player->score += enemies[i]->score;
						}
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y + 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x, enemies[i]->position.y, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x + 5, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->particles->AddParticle(App->particles->explosion, enemies[i]->position.x - 15, enemies[i]->position.y - 10, 0, 0, COLLIDER_NONE);
						App->audio->PlayFX(audio_explosion4);
						App->enemies->AddEnemy(POWER_UP, enemies[i]->position.x, enemies[i]->position.y);
						App->collision->rwing = true;
						App->collision->EraseCollider(enemies[i]->collider);
						delete enemies[i];
						enemies[i] = nullptr;
					}
				}

			}
		}
	}
}