#include "Application.h"
#include "Enemy_FlyingMachine2.h"
#include "ModuleCollision.h"
#include "ModuleTextures.h"
#include "ModuleParticles.h"
#include "ModulePlayer.h"
#include "SDL/include/SDL_timer.h"

#include <math.h>



Enemy_FlyingMachine2::Enemy_FlyingMachine2(int x, int y) : Enemy(x, y)
{
	enemy_position.PushBack({ 626,105,32,34 });
	enemy_position.speed = 0.2f;
	animation = &enemy_position;


	fly.PushBack({ 531,154,32,32 });
	fly.PushBack({ 568,152,32,32 });
	fly.PushBack({ 601,152,32,32 });
	fly.PushBack({ 638,151,32,32 });
	fly.PushBack({ 684,152,32,32 });
	fly.PushBack({ 737,153,32,32 });

	fly.speed = 0.2f;
	animation2 = &fly;



	collider = App->collision->AddCollider({ 0, 0,26,26 }, COLLIDER_TYPE::COLLIDER_ENEMY, (Module*)App->enemies);

	movement.PushBack({ 0.0f,-0.775f }, 250, &enemy_position);	
	movement.PushBack({ 0.1f,-0.1f }, 650, &enemy_position);

	originalpos.x = x;
	originalpos.y = y;


}

void Enemy_FlyingMachine2::Move()
{

	position = originalpos + movement.GetCurrentPosition();

}

void Enemy_FlyingMachine2::Shoot() {

	now = SDL_GetTicks() - start_time;
	if (now > shoots * 1000) {
		shootspeed_x = (App->player->position.x - (position.x));
		shootspeed_y = (App->player->position.y - (position.y));

		vmodule = sqrt((shootspeed_x*shootspeed_x) + (shootspeed_y*shootspeed_y));

		shootspeed_x_u = (shootspeed_x / vmodule) * 5;
		shootspeed_y_u = (shootspeed_y / vmodule) * 5;


		if (shootspeed_y_u >= 0) {
			App->particles->AddParticle(App->particles->enemy_shoot, position.x + 21, position.y + 26, shootspeed_x_u, shootspeed_y_u - 1.88f, COLLIDER_ENEMY_SHOT);

		}
		shoots++;
	}

}