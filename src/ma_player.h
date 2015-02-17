/****************************
Copyright © 2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


#ifndef MA_PLAYER_HPP
#define MA_PLAYER_HPP

typedef enum {
	NOTRUNNING,
	RUNNING,
	PAUSED,
	NOUPDATE,
	GUIMODE,
	IDLE,
	SAVING,
	LOADING,
	RESETGAME,
	EXITING,
	INVALIDPOSITION,
	RELOADENTITIES,
	GAMEERROR = -1
} LuxState;

typedef struct
{
	GdkNativeWindow winId;
	LuxState state;
	GtkWidget * widget;
	GPid pid;

} PlayerInfo;

void MegWidget_Player_Play(GtkWidget *button, PlayerInfo *player_info);
void MegWidget_Player_Pause( GtkWidget *button, PlayerInfo *player_info);
void MegWidget_Player_Refresh(GtkWidget *button, PlayerInfo *player_info);

#endif // MA_PLAYER_HPP
