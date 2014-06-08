#if defined _map_included
	#endinput
#endif
#define _map_included

//title Map Functions
//description Maps are split into two modes, a Grid based Section which allows map switching by moving off the map and a Independent File Mode.


native MapIdent(); // Use CURRENT_MAP instead.

#define MAP_ERROR 0

#define GLOBAL_MAP 0
#define CURRENT_MAP MapIdent()


///Mask Functions 
/***
<function name="MaskRefresh">
	<summary>Updates Mask from current Map Objects.</summary>
	<example>
	function Init( ... )
	{
	   while (n &lt; 5)
		  ObjectCreate("image.png:wall", SPRITE);

	   MaskRefresh();
	}
</example>
</function>
<function name="MaskGetValue">
	<summary>Returns Mask value.</summary>
	<param name="x" default="" type="n">the point you wish to check.</param>
	<param name="y" default="" type="n">the point you wish to check.</param>
	<param name="layer" default="0" type="n">Not used</param>
	<return>Value of mask at that point. Value is capped between 0-255.</return>
</function>
<function name="MaskFill">
	<summary>Fills area of mask.</summary>
	<param name="x" default="" type="n">Area of Mask to change.</param>
	<param name="y" default="" type="n">Area of Mask to change.</param>
	<param name="w" default="" type="n">Area of Mask to change.</param>
	<param name="h" default="" type="n">Area of Mask to change.</param>
	<param name="value" default="0" type="n">Set the mask are to this value.</param>
</function>
***/
native MaskRefresh();
native MaskGetValue(x, y, layer = 0);
native MaskFill(x, y, w, h, value = 0);


///Map Functions
/***
<function name="MapSetOffset">
	<summary>Set the Offset from the Top-left of the map.</summary>
	<param name="Fixed:x" default="" type="n"></param>
	<param name="Fixed:y" default="" type="n"></param>
</function>
<function name="MapGetOffset">
	<summary>Get the Offset from the Top-left of the map.</summary>
	<param name="axis" default="" type="n"></param>
</function>
***/
native MapSetOffset(Fixed:x, Fixed:y);
native Fixed:MapGetOffset(axis);

///Map File Code
/***
<function name="MapCreate">
	<summary>Create a new map.</summary>
	<param name="map[]" default="" type="s">Name of file.</param>
	<param name="create_new" default="0" type="n">create new map.</param>
	<return>New map ID.</return>
</function>
<function name="MapChange">
	<summary>Changes to map id.</summary>
	<param name="mapid" default="" type="n">Map ID to move to.</param>
	<param name="offsetx" default="-1" type="n">Offset position.</param>
	<param name="offsety" default="-1" type="n">Offset position.</param>
	<return>Map ID. ) if error</return>
</function>
<function name="MapValid">
	<summary>Check if map id is valid.</summary>
	<param name="mapid" default="" type="n">map id to check.</param>
	<return>True if valid.</return>
</function>
<function name="MapCurrent">
	<summary>Get Current Map ID and name. Note: it will load map.</summary>
	<param name="map[]" default="" type="a">Sting to store name of map.</param>
	<return>Map ID.</return>
</function>
<function name="MapID">
	<summary>Get Map ID. Note: it will load map.</summary>
	<param name="map[]" default="SELF_STRING" type="a">Name of file.</param>
	<return>Map ID.</return>
</function>
<function name="MapSave">
	<summary>Save the map</summary>
</function>
***/
native MapCreate( map{}, create_new = 0, maxlength = sizeof map );
native MapChange(mapid, offsetx = -1, offsety = -1);
native MapValid(mapid);
native MapID(map{}, maxlength=sizeof map);
native MapCurrent(map{} = SELF_STRING, maxlength=sizeof map);
native MapSave();
native MapSaveToFile();

///Map Section Function
/***
<function name="SectionSet">
	<summary>Changes the Section, while keeping the same grid location.</summary>
	<param name="section[]" default="" type="s">Section name.</param>
	<param name="gridx" default="-1" type="n">Grid Position</param>
	<param name="gridy" default="-1" type="n">Grid Position</param>
</function>
<function name="SectionGet">
	<summary>Get the current section name.</summary>
	<param name="section[]" default="" type="a">Section name.</param>
	<param name="&amp;gridx" default="-1" type="n">Grid Position</param>
	<param name="&amp;gridy" default="-1" type="n">Grid Position</param>
</function>
<function name="SectionValid">
	<summary>Check if a grid location on a section is valid.</summary>
	<param name="name[]" default="" type="s"></param>
	<param name="gridx" default="-1" type="n"></param>
	<param name="gridy" default="-1" type="n"></param>
</function>
<function name="SectionLoad">
	<summary>Loads a section</summary>
	<param name="name[]" default="" type="s"></param>
</function>
***/
native SectionSet(section{}, gridx = -1, gridy = -1, maxlength = sizeof section );
native SectionGet(section{}, &gridx, &gridy, maxlength=sizeof section);
native SectionValid(section{} = SELF_STRING, gridx, gridy, maxlength = sizeof section );
native SectionLoad(section[], unload = 0, maxlength = sizeof section );
