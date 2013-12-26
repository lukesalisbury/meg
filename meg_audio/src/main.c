/****************************
Copyright (c) 2010 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gmodule.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_thread.h>


G_MODULE_EXPORT int sdl_playback_play( gchar * file, void (*music_finished)() )
{
	Mix_Music * music = Mix_LoadMUS( file );

	if ( music == NULL )
	{
		g_warning( "Meg_Audio: %s\n", Mix_GetError() );
		return 0;
	}
	Mix_PlayMusic( music, 1 );
	Mix_HookMusicFinished( music_finished );
	return 1;
}

G_MODULE_EXPORT void sdl_playback_quit()
{
	Mix_CloseAudio();
	SDL_Quit();
}

G_MODULE_EXPORT void sdl_playback_init()
{
	if ( SDL_Init( SDL_INIT_AUDIO ) == -1 )
	{
		g_warning( "Meg_Audio: %s\n", SDL_GetError() );
	}

	if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 )
	{
		g_warning( "Meg_Audio: %s\n", Mix_GetError() );
	}

}

#if __GNUWIN32__
#include <windows.h>
BOOL APIENTRY DllMain ( HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
#endif
