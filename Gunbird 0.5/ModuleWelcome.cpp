#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleCharacterSelect.h"
#include "ModuleWelcome.h"
#include "ModuleSea.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleCollision.h"
#include "ModuleFadeToBlack.h"



ModuleWelcome::ModuleWelcome()
{
	// Background 
	welcome.x = 0;
	welcome.y = 0;
	welcome.w = 224;
	welcome.h = 320;

	//start animation
	gameover.PushBack({ 2,91,124,31 });
	gameover.PushBack({ 0,0,0,0 });
	gameover.speed = 0.013f;


}

ModuleWelcome::~ModuleWelcome()
{}

// Load assets
bool ModuleWelcome::Start()
{
	LOG("Loading background assets");
	bool ret = true;
	welcometexture = App->textures->Load("assets/background_welcome.png");
	gameovertexture = App->textures->Load("assets/ui.png");
	App->audio->LoadMusic("assets/Audio/characterselection.ogg");
	


	return ret;
}

bool ModuleWelcome::CleanUp()
{
	App->textures->Unload(welcometexture);
	App->textures->Unload(gameovertexture);
	App->audio->UnloadMusic();
	App->welcome->Disable();
	LOG("Unloading stage");
	return true;
}

// Update: draw background
update_status ModuleWelcome::Update()
{

	// Draw everything --------------------------------------


	App->render->Blit(welcometexture, 0, -320 + SCREEN_HEIGHT, &welcome, 0.75f);

	App->render->Blit(gameovertexture, 50, 200, &(gameover.GetCurrentFrame()), 0.22f);

	

	if (App->input->keyboard[SDL_SCANCODE_P] && change || App->input->buttonA == KEY_STATE::KEY_DOWN && change) {

		change = false;
		App->fade->FadeToBlack(this, App->characterselect, 1);
		change = true;
		//App->input->buttonA = false;
	}



	return UPDATE_CONTINUE;
}