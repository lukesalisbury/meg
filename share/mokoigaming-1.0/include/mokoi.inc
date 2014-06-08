#if defined _project_included
	#endinput
#endif

#define _project_included

//title Main Functions

#include <fixed>
#include <enums>

native printf(const format[], ...);

///Engine Functions 
/***
<function name="KeyboardWatch">
	<summary>Send Keyboard input to KeyboardInput function in the current entity.</summary>
	<param name="able" default="" type="n"></param>
	<example>
	forward public KeyboardInput( unicode );
	public KeyboardInput( unicode )
	{
		DebugText("Input: %c %d", unicode, unicode)
	}

	public Init( ... )
	{
		KeyboardWatch(true);
	}
	</example>
</function>
<function name="GameState">
	<summary>Get/Set Global State.</summary>
	<param name="newstate" default="-1" type="n">Set new engine state, -1 to skip.</param>
	<return>Current state of the engine.</return>
	<example>
	if ( GameState() == 0 )
	{
		if ( InputButton(0) )
		{
			GameState(1);
		}
	}
	else
	{
		MoveEntity();
	}
	</example>
</function>
<function name="GameFrame">
	<summary>Return the length of last frame in milliseconds.</summary>
	<return>Length of time of last frame.</return>
	<example>
	new time = GameFrame();
	DebugText("Last Frame took %d milliseconds", time);
	</example>
</function>
<function name="GameFrame2">
	<summary>Return the length of last frame in seconds as a fixed point number.</summary>
	<return>Length of time of last frame.</return>
	<example>
	new Fixed:time = GameFrame2();
	DebugText("Last Frame took %q seconds", time);
	</example>
</function>
***/
native KeyboardWatch( able );
native GameState(newstate = -1);
native GameFrame();
native Fixed:GameFrame2() = GameFrame;

///Log Functions
/***
<function name="GameLog">
	<summary>Prints formatted text to a text file.</summary>
	<param name="text[]" default="" type="a">Format string.%c print a character at this position. %d print a number at this position in decimal radix. %f print a floating point number at this position. %q print a fixed point number at this position. %s print a character string at this position. %x print a number at this position in hexadecimal radix</param>
	<param name="..." default="" type="v">List of Arguments to used to produce the formatted text</param>
	<example>
	GameLog("Doing Something :)");
	GameLog("Text: %s, Color: %x, Number: %d, Fixed: %q\n", "Hello", 0xFFBBAAFF, 42, 50.254); // Outputs Text: Hello, Color: FFBBAAFF, Number: 42, Fixed: 50.254
	GameLog("Text: %s, Color: %d, Number: %05d, Fixed: %.1q\n", "Hello", 0xFFBBAAFF, 42, 50.254); // Outputs Text: Hello, Color: -4478209, Number: 00042, Fixed: 50.3
	</example>
</function>
<function name="DebugText">
	<summary>Prints formatted text to the debug console.</summary>
	<param name="text[]" default="" type="a">Format string. %c print a character at this position. %d print a number at this position in decimal radix. %f print a floating point number at this position. %q print a fixed point number at this position. %s print a character string at this position. %x print a number at this position in hexadecimal radix</param>
	<param name="..." default="" type="v">List of Arguments to used to produce the formatted text.</param>
	<example>
	DebugText("Doing Something :)");
	DebugText("Text: %s, Color: %x, Number: %d, Fixed: %q\n", "Hello", 0xFFBBAAFF, 42, 50.254); // Outputs Text: Hello, Color: FFBBAAFF, Number: 42, Fixed: 50.254
	DebugText("Text: %s, Color: %d, Number: %05d, Fixed: %.1q\n", "Hello", 0xFFBBAAFF, 42, 50.254); // Outputs Text: Hello, Color: -4478209, Number: 00042, Fixed: 50.3
	</example>
</function>

***/
native GameLog( text[], {Fixed,_}:... ) = printf;
native DebugText( text[], {Fixed,_}:... );

///Save Functions
/***
<function name="GameSave">
	<summary>Save Current Game.</summary>
	<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
	<param name="detail[64]" default="" type="a">256 bytes of extra save data, for uses such as high scores, player name, time of save. This can be access via other games if feature is enable.</param>
</function>
<function name="GameLoad">
	<summary>Load Game from Slot.</summary>
	<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
</function>
<function name="GameSaveHibernate">
	<summary>Hibernate a game</summary>
</function>
<function name="GameGetDetails">
	<summary>Get Save Game Details.</summary>
	<param name="slot" default="" type="n">Save slot to use, 0-9 are available.</param>
	<param name="array[64]" default="" type="a">Array to store the information in.</param>
	<example>
	new array[64];
	GameGetDetails(0, array);
	highscore = array[0];
	</example>
</function>
<function name="GameDetails">
	<summary>Get another game save details.</summary>
	<param name="gameid" default="" type="n">ID to the game you wish to access.</param>
	<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
	<param name="array[64]" default="" type="a">Array to store the information in.</param>
	<example>
	new array[64];
	GameDetails(prequel_id, 0, array);
	played_prequel = array[0];
	</example>
</function>
<function name="GameHasSave">
	<summary>Check if file exist.</summary>
	<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
</function>
<function name="GameRemoveSave">
	<summary>Remove Save Game.</summary>
	<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
</function>
***/
native GameLoad(slot);
native GameSaveHibernate();
native GameSave(slot, detail[64],  maxlength=sizeof detail);
native GameGetDetails(slot, array[64], bool:pack_array = false, maxlength=sizeof array);
native GameDetails(gameid, slot, array[64], bool:pack_array = false, maxlength=sizeof array);

native GameHasSave(slot);
native GameRemoveSave(slot);


///Input Functions
/***
<function name="InputButton">
	<summary>Returns Button value for Player.</summary>
	<param name="input" default="" type="n">A value between 0-15.</param>
	<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
	<return>0 for un-pressed button, 1 for pressed button, 2 for held down button</return>
	<example>
	if ( InputButton(0,0) == 1 )
		Shoot();
	</example>
</function>
<function name="InputAxis">
	<summary>Returns Axis value for Player.</summary>
	<param name="axis" default="" type="n">A value between 0-5, this representing x1,y1,z1,x2,y2,z2 axis.</param>
	<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
	<return>Value between -255 and 255.</return>
	<example>
	if ( InputButton(1,0) == 255 )
		MoveUp();
	</example>
</function>
<function name="InputPointer">
	<summary>Returns Pointer value for Player.</summary>
	<param name="axis" default="" type="n">0 for x axis, 1 for y axis.</param>
	<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
	<return>Value the current value of axis of mouse or touchscreen.</return>
	<example>
	if ( InputButton(0,0) == 1 )
		ShootAt(InputPointer(0,0), InputPointer(1,0));
	</example>
</function>
<function name="InputButtonSet">
	<summary>Set Button value for the Player.</summary>
	<param name="input" default="" type="n">A value between 0-15.</param>
	<param name="value" default="" type="n">0 for un-pressed button, 1 for pressed button, 2 for held down button</param>
	<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
</function>
<function name="InputAxisSet">
	<summary>Set Axis value for the Player.</summary>
	<param name="axis" default="" type="n">A value between 0-5, this representing x1,y1,z1,x2,y2,z2 axis.</param>
	<param name="value" default="" type="n">Value between -255 and 255.</param>
	<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
</function>
<function name="InputPointerSet">
	<summary>Set Pointer value for the Player.</summary>
	<param name="axis" default="" type="n">0 for x axis, 1 for y axis.</param>
	<param name="value" default="" type="n"></param>
	<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
</function>
***/
native InputButton(input, player = 0);
native InputAxis(axis, player = 0);
native InputPointer(axis, player = 0);
native InputButtonSet(input, value, player = 0);
native InputAxisSet(axis, value, player = 0);
native InputPointerSet(axis, value, player = 0);
native InputSetDefault(player);

///Misc Functions 
/***
<function name="MiscGetWidth">
	<summary>Get Width of Map, Screen or Sprite.</summary>
	<param name="id[]" default="" type="a">Name of sprite or "__screen" for screen, "__map" for standard map and "map" for current map.</param>
	<return>Width of Map, Screen or Sprite.</return>
</function>
<function name="MiscGetHeight">
	<summary>Get Height of Map, Screen or Sprite.</summary>
	<param name="id[]" default="" type="a">Name of sprite or "__screen" for screen, "__map" for standard map and "map" for current map.</param>
	<return>Height of Map, Screen or Sprite.</return>
</function>
***/
native MiscGetWidth(id[]);
native MiscGetHeight(id[]);

///Language Functions
/***
<function name="LanguageString">
	<summary>Get String from Language File.</summary>
	<param name="line" default="" type="n">line of language wanted.</param>
	<param name="returnstring[]" default="" type="a">String to store language string.</param>
</function>
<function name="LanguageSet">
	<summary>Loads a new language set. note: previous strings may have updated.</summary>
	<param name="lang[]" default="" type="a">Two letter language code </param>
</function>
***/
native LanguageString(line, returnstring[], maxlength=sizeof returnstring);
native LanguageSet(lang[], maxlength=sizeof lang);

///Dialog Functions
/***
<function name="DialogShow">
	<summary>Shows Dialog box.</summary>
	<param name="line" default="" type="n">the requested line.</param>
	<param name="callback[]" default="&quot;dialogbox&quot;" type="a">Function to call in the main script.</param>
</function>
<function name="DialogGetString">
	<summary>Get String from Dialog File.</summary>
	<param name="line" default="" type="n">the requested line.</param>
	<param name="returnstring[]" default="" type="a"></param>
</function>
<function name="DialogPlayAudio">
	<summary>Play Dialog.</summary>
	<param name="line" default="" type="n">the requested line.</param>
</function>
***/
native DialogShow(line, callback{} = "dialogbox");
native DialogGetString(line, returnstring[], maxlength=sizeof returnstring);
native DialogPlayAudio(line);

///Audio Functions 
/***
<function name="AudioVolume">
	<summary>Sets Audio device volume.</summary>
	<param name="AudioDevice:device" default="" type="n">The Audio device you wish to change.</param>
	<param name="level" default="-1" type="n">Requested Volume level.</param>
	<return>Current volume level</return>
</function>
<function name="AudioPlaySound">
	<summary>Play Sound Effect. Also return length of file in seconds.</summary>
	<param name="file[]" default="" type="a">filename of the sound effect.</param>
	<param name="x" default="-1" type="n">X-axis screen position of the sound.</param>
	<param name="y" default="-1" type="n">y-axis screen position of the sound.</param>
	<return>Length of the audio file.</return>
</function>
<function name="AudioPlayMusic">
	<summary>Play Music.</summary>
	<param name="file[]" default="" type="a">Filename of the music you wish to play.</param>
	<param name="loop" default="1" type="n">Set to false to play music only once.</param>
	<param name="fadeLength" default="0" type="n">The length of fade in milliseconds</param>
</function>
***/
native AudioVolume(AudioDevice:device, level = -1);
native AudioPlaySound(file{}, x = -1, y = -1);
native AudioPlayMusic(file{}, loop = 1, fadeLength = 0);

///Config Functions
/***
<function name="ConfigGetString">
	<summary>Get Engine config as a string.</summary>
	<param name="config[]" default="" type="a"></param>
	<param name="value[]" default="" type="a"></param>
</function>
<function name="ConfigGetNumber">
	<summary>Get Engine config as a number.</summary>
	<param name="config[]" default="" type="a"></param>
	<return>A number from game settings, 0 if no value is found or not is not a number.</return>
</function>
<function name="ConfigSetString">
	<summary>Update Engine config.</summary>
	<param name="config[]" default="" type="a"></param>
	<param name="value[]" default="" type="a"></param>
</function>
***/
native ConfigGetString(config{}, value{}, maxlength=sizeof value);
native ConfigGetNumber(config{});
native ConfigSetString(config{}, value{});

///File Listing Functions
/***
<function name="FileGetList">
	<summary>List File in a directory.</summary>
	<param name="strings[][]" default="" type="a">Stores file name in directory.</param>
	<param name="directory[]" default="&quot;map&quot;" type="a">Directory name.</param>
</function>
***/
native FileGetList(strings[][], directory{} = "map", size=sizeof strings);
