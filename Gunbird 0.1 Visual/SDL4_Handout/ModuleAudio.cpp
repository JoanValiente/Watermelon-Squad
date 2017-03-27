#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer/include/SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

ModuleAudio::ModuleAudio() : Module()
{
}

// Destructor
ModuleAudio::~ModuleAudio()
{
}

// Called before render is available
bool ModuleAudio::Init()
{
	bool ret = true;
	Mix_Init(MIX_INIT_OGG);
	music = Mix_LoadMUS("Audio/mine.ogg");
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_PlayMusic(music, -1);
	
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0){
		LOG("Audio could not initialize. SDL_ERROR: %s", Mix_GetError());
		ret = false;
	}
	if (music == NULL) {
		LOG("Error loading music: %s", Mix_GetError());
		ret = false;
	}
	if (Mix_PlayMusic(music, -1) != 0) {
		LOG("Error playing music: %s", Mix_GetError());
		ret = false;
	}

	return ret;
}
	

// Called before q	uitting
bool ModuleAudio::CleanUp()
{
	if (music != NULL) {
		Mix_FreeMusic(music);
	}
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}


