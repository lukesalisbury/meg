#if defined _enum_included
  #endinput
#endif
#define _enum_included



#define SELF 0
#define SELF_STRING ""

#define OBJECT_NONE object:-1

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

#define POSITION_MAP 0
#define POSITION_SCREEN 1

const Directions: {
	SOUTH = 0,	//0 deg
	SOUTHEAST,	//45 deg
	EAST,		//90 deg
	NORTHEAST,	//135 deg
	NORTH,		//180 deg
	NORTHWEST,	//225 deg
	WEST,		//270 deg
	SOUTHWEST	//315 deg
}

const AudioDevice:{
	SOUNDFX = 0,
	DIALOG, // Dialog most likely share the SOUNDFX channels
	MUSIC
}

const DisplayDevice: {
	UNKNOWN = 0,
	SOFTWARE, // Rotating, scaling & colouring are done via Software. 
	HARDWARE, // Uses OpenGL/Direct3D SDL 2.0 backends. 
	OPENGL, // If supported, Rotating, scaling & colouring are done via the Video Card.
}

const GraphicTypes: {
	SPRITE = 's',
	RECTANGLE = 'r',
	LINE = 'l',
	TEXT = 't',
	POLYGON = 'p',
	CIRCLE = 'c',
	CANVAS = 'M',
}

const StyleTypes: {
	STYLE_NORMAL = 0,
	STYLE_VGRADIENT,
	STYLE_HGRADIENT,
	STYLE_GLOW,
	STYLE_OUTLINE,
	STYLE_REPLACECOLOUR,
	STYLE_NEGATIVE
}

const ObjectFlags: {
	FLAG_SPEED = 0,
	FLAG_ANIMLOOP = 1,
	FLAG_ANIMREVERSE = 2,
	FLAG_ANIMRESET = 3,
	FLAG_HIDDEN = 6
}


#define WHITE 0xFFFFFFFF
#define BLACK 0x000000FF
#define RED 0xFF0000FF
#define BLUE 0x0000FFFF
#define GREEN 0x00FF00FF
