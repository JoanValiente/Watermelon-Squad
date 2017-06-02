#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleWelcome.h"
#include "ModulePlayer.h"
#include "ModulePlayer2.h"
#include "ModuleRender.h"
#include "ModuleSea.h"
#include "ModuleBoss.h"
#include "ModuleAudio.h"
#include "ModuleParticles.h"
#include "ModuleCollision.h"
#include "ModuleInput.h"
#include "ModuleFadeToBlack.h"
#include "ModuleEnemies.h"
#include "ModuleCongrats.h"
#include "ModuleCharacterSelect.h"
#include "ModuleUI.h"


ModuleBoss::ModuleBoss()
{
	// Background 

	boss_animation.PushBack({ 0, 0, 288, 640 });
	boss_animation.PushBack({ 288, 0, 288, 640 });
	boss_animation.PushBack({ 576, 0, 288, 640 });
	boss_animation.PushBack({ 864, 0, 288, 640 });
	boss_animation.PushBack({ 1152, 0, 288, 640 });
	boss_animation.PushBack({ 864, 0, 288, 640 });
	boss_animation.PushBack({ 576, 0, 288, 640 });
	boss_animation.PushBack({ 288, 0, 288, 640 });
	boss_animation.speed = 0.1;


	boss_animation2.PushBack({ 0, 0, 288, 640 });
	boss_animation2.PushBack({ 288, 0, 288, 640 });
	boss_animation2.PushBack({ 576, 0, 288, 640 });
	boss_animation2.PushBack({ 864, 0, 288, 640 });
	boss_animation2.PushBack({ 1152, 0, 288, 640 });
	boss_animation2.PushBack({ 864, 0, 288, 640 });
	boss_animation2.PushBack({ 576, 0, 288, 640 });
	boss_animation2.PushBack({ 288, 0, 288, 640 });
	boss_animation2.speed = 0.1;



}

ModuleBoss::~ModuleBoss()
{}

// Load assets
bool ModuleBoss::Start()
{

	LOG("Loading background assets");

	pause = false;
	boss_speed = 0.22f;
	boss_y = -640;
	boss_y2 = -1280;


	bosstexture = App->textures->Load("assets/boss_background.png");
	startplayer2texture = App->textures->Load("assets/ui.png");

	App->audio->LoadMusic("assets/Audio/boss.ogg");



	App->ui->destroyed = false;
	App->particles->Enable();
	App->collision->Enable();
	App->enemies->Enable();
	App->player->Enable();

	if (App->characterselect->coop == true) {
		App->player2->Enable();
	}

	App->ui->Enable();




	//Enemy


	//App->enemies->AddEnemy(ENEMY_TYPES::BIRDBODY, SCREEN_WIDTH / 2 -44, 50);
	//App->enemies->AddEnemy(ENEMY_TYPES::LEFTWINGBIRD, SCREEN_WIDTH / 2 - 52, 102);
	//App->enemies->AddEnemy(ENEMY_TYPES::RIGHTWINGBIRD, SCREEN_WIDTH / 2 + 89, 102);

	App->enemies->AddEnemy(ENEMY_TYPES::SHIP, 10, -407);

	//FIRST STATE

	App->enemies->AddEnemy(ENEMY_TYPES::SHIPTURRET, 16, -288);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPTURRET, 146, -288);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPTURRET, 9, -177);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPTURRET, 152, -177);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPTURRET, 124, -151);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPTURRET, 38, -151);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPTURRET, 50, -73);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPTURRET, 112, -73);

	App->enemies->AddEnemy(ENEMY_TYPES::SHIPBIGMIDDLETURRET, 28, -276);

	App->enemies->AddEnemy(ENEMY_TYPES::SHIPVERTICALCANON,66, -192);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPVERTICALCANON, 116, -192);

	App->enemies->AddEnemy(ENEMY_TYPES::SHIPHORIZONTALCANON, 86, -144);

	//SECOND STATE

	App->enemies->AddEnemy(ENEMY_TYPES::SHIPVERTICALCANON2, 16, -288);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPVERTICALCANON3, 146, -288);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPVERTICALCANONLITTLE, 9, -177);
	App->enemies->AddEnemy(ENEMY_TYPES::SHIPBACKTURRET, 35, -437);







	




	return true;
}

bool ModuleBoss::CleanUp()
{

	App->enemies->Disable();
	App->collision->Disable();
	App->particles->Disable();
	App->player->Disable();
	App->player2->Disable();
	App->ui->Disable();
	App->textures->Unload(bosstexture);
	App->textures->Unload(startplayer2texture);
	App->audio->UnloadMusic();
	App->boss->Disable();



	LOG("Unloading stage");
	return true;
}

// Update: draw background
update_status ModuleBoss::Update()
{

	// -------------------------------------- Draw everything --------------------------------------
	App->render->Blit(bosstexture, -64, boss_y + SCREEN_HEIGHT, &(boss_animation.GetCurrentFrame()), boss_speed);
	App->render->Blit(bosstexture, -64, boss_y2 + SCREEN_HEIGHT, &(boss_animation.GetCurrentFrame()), boss_speed);

	if (App->player->dead == false) {
		if (App->input->keyboard[SDL_SCANCODE_P] == KEY_STATE::KEY_DOWN || App->input->buttonStart == KEY_STATE::KEY_DOWN) {
			if (pause == false) {
				pause = true;
			}
			else if (pause == true) {
				pause = false;
			}
		}
	}
	if (pause == true) {
		App->render->camera.y -= SCROLL_SPEED;
		App->player->position.y += 1;
		App->player->camera_limits.y += 1;
	}

	//Background--------------------------

	if (App->input->keyboard[SDL_SCANCODE_U] == KEY_STATE::KEY_DOWN) {
		LOG("%i", App->render->camera.y);
	}

	if (App->render->camera.y == 5800)
	{
		boss_y -= 1280;
	}
	if (App->render->camera.y == 11600)
	{
		boss_y2 -= 1280;
	}

	//UI--------------------------


	if (App->render->camera.y <= 10000000) {
		App->render->camera.y += SCROLL_SPEED;
	}

	else {
		change = false;
		App->fade->FadeToBlack(this, App->congrats, 1);
		change = true;

	}
	if (App->input->keyboard[SDL_SCANCODE_F7] == KEY_STATE::KEY_DOWN) {
		change = false;
		App->fade->FadeToBlack(this, App->congrats, 1);
		change = true;
	}

	//ENEMIES--------------------------


	return UPDATE_CONTINUE;
}