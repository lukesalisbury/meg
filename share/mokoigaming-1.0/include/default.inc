#include <mokoi>
#include <graphics>
#include <entities>
#include <fixed>
#include <string>
#include <map>

#tryinclude <game_default>
#include <controller>


forward public Init(...);
forward public UpdatePosition();
forward public Close();

forward public AutoPreInit();
forward public AutoPostInit();
forward public AutoPreClose();
forward public AutoPostClose();
forward public AutoUpdatePosition();
