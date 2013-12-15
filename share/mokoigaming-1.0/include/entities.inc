#if defined _entities_included
  #endinput
#endif
#define _entities_included

//title Entity Functions

#include <enums>
#include <fixed>


#define ARG_NUMBER	'n'
#define ARG_RETURN_NUMBER	'd'
#define ARG_ARRAY	'a'
#define ARG_STRING	's'
#define ARG_END	'\0'

///Entity Position Functions
/***
<function name="EntitySetPosition">
	<summary>Set Entities position on current map.</summary>
	<param name="Fixed:x" default="_" type="n">New Position. Pass cellmin to skip update that value.</param>
	<param name="Fixed:y" default="_" type="n">New Position. Pass cellmin to skip update that value.</param>
	<param name="Fixed:z" default="_" type="n">New Position. Pass cellmin to skip update that value.</param>
	<param name="id" default="_" type="n">Entity ID.</param>
</function>
<function name="EntityGetPosition">
	<summary>Get Entity position on current map.</summary>
	<param name="&amp;Fixed:x" default="" type="n">Variable to store Position.</param>
	<param name="&amp;Fixed:y" default="" type="n">Variable to store Position.</param>
	<param name="&amp;Fixed:z" default="" type="n">Variable to store Position.</param>
	<param name="id" default="_" type="n">Entity ID.</param>
</function>
***/
native EntitySetPosition(Fixed:x = Fixed:cellmin, Fixed:y = Fixed:cellmin, Fixed:z = Fixed:cellmin, id = SELF);
native EntityGetPosition(&Fixed:x, &Fixed:y, &Fixed:z, id = SELF);


///Entity Functions
/***
<function name="EntityHash">
	<summary>Convert Entity Name to hash</summary>
	<param name="id_string[]" default="" type="a">ID to give the entity.</param>
	<return>Returns the Entity Hash.</return>
</function>
<function name="EntityCreate">
	<summary>Creates a new entity.</summary>
	<param name="parententity[]" default="&quot;&quot;" type="a">Entity File name.</param>
	<param name="id_string[]" default="&quot;&quot;" type="a">ID to give the entity.</param>
	<param name="x" default="" type="n">position to give the the entity.</param>
	<param name="y" default="" type="n">position to give the the entity.</param>
	<param name="z" default="" type="n">position to give the the entity.</param>
	<param name="map_id" default="CURRENT_MAP" type="n">Map to create it on. CURRENT_MAP will created it on the current map. GLOBAL_MAP with create a global entity which always on.</param>
	<param name="args[]" default="''''" type="a">Defines the follow arguments that will be passed to the entity Init function.
	 a - Array, last value must be CELLMIN
	 d - Value, passed by reference
	 n - Value, passed by value
	 s - String
	 "snsn" means that the follow argument you pass will be a string, a number, a string and a number.
	</param>
	<param name="..." default="" type="v">List of Arguments to pass to the entity Init function. </param>
	<return>0 on error, otherwise it returns the Entity Hash.</return>
</function>
<function name="EntityDelete">
	<summary>Deletes a entity.</summary>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
</function>
<function name="EntityGetSetting">
	<summary>Get stored setting for entity. These are set via the Runtime Options.</summary>
	<param name="key[]" default="" type="a">Name of Setting.</param>
	<param name="string[]" default="" type="a">String to store value in.</param>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
</function>
<function name="EntityGetNumber">
	<summary>Returns stored setting for entity. These are set via the Runtime Options.</summary>
	<param name="key[]" default="" type="a">Name of Setting.</param>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
</function>
<function name="EntityGetObject">
	<summary>Returns "object-id" value for entity, tagged as object. </summary>
</function>
<function name="EntityPublicFunction">
	<summary>Calls a function from another entity.</summary>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
	<param name="function[]" default="" type="a">Public Function Name.</param>
	<param name="args[]" default="''''" type="a">Defines the follow arguments that will be passed to the function.
	 a - Array, last value must be CELLMIN
	 d - Value, passed by reference
	 n - Value, passed by value
	 s - String
	 ''snsn'' means that the follow argument you pass will be a string, a number, a string and a number.
	</param>
	<param name="..." default="" type="v">List of Arguments to pass to the function.</param>
	<return>What the function returns. May return -1 and -2 on error. </return>
</function>
<function name="EntityPublicVariable">
	<summary>Returns a public variable from entity.</summary>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
	<param name="variable[]" default="" type="a">Name of public variable.</param>
	<return>the public variable value.</return>
</function>
<function name="EntityPublicVariableSet">
	<summary>Sets a public variable on a entity.</summary>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
	<param name="variable[]" default="" type="a">Name of public variable.</param>
	<param name="value" default="" type="n">new value.</param>
</function>
***/
native EntityHash(string{}="", maxlength = sizeof(string));
native EntityCreate(parententity{}, id_string{}, Fixed:x, Fixed:y, Fixed:z, map_id, args[]='''', {Fixed,_}:...);
native EntityDelete(id = SELF);
native EntityGetSetting(key{}, string{}, id = SELF);
native EntityGetSettingHash(key{}, id = SELF);
native EntityGetNumber(key{}, id = SELF );
native object:EntityGetObject(key{} = "object-id", id = SELF ) = EntityGetNumber;
// Args 
/*
 a - Array, last value must be CELLMIN
 d - Value, passed by reference
 n - Value, passed by value
 s - String
*/
native EntityPublicFunction(id, function[], args[]='''', ...);
native EntityPublicVariable(id, variable[]);
native EntityPublicVariableSet(id, variable[], value);

///Entity Collision Functions
/***
<function name="CollisionFromObject">
	<summary>Set Collision rectangles from the sprite or animation data.</summary>
	<param name="obj" default="" type="n">Display Object to use.</param>
	<param name="type" default="0" type="n">Collision rectangle type.</param>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
</function>
<function name="CollisionCheck">
	<summary>Check to see if Entity is colliding with another entity.</summary>
	<param name="id1" default="" type="n">The Hash ID of the first entity you wish to check.</param>
	<param name="id2" default="" type="n">The Hash ID of the second entity you wish to check.</param>
	<param name="rect1" default="-1" type="n">The collision rectangle of first entity you wish to check.</param>
	<param name="rect2" default="-1" type="n">Rhe collision rectangle of second entity you wish to check.</param>
	<return>True if the collision rectangle of two entities are colliding.</return>
</function>
<function name="CollisionSet">
	<summary>Set a collision for the entity.</summary>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
	<param name="rect" default="0" type="n">Collision Rectangle.</param>
	<param name="type" default="0" type="n">Collision Type.</param>
	<param name="x" default="0" type="n">Rectangle points. </param>
	<param name="y" default="0" type="n">Rectangle points. </param>
	<param name="w" default="0" type="n">Rectangle points. </param>
	<param name="h" default="0" type="n">Rectangle points. </param>
</function>
<function name="CollisionCalculate">
	<summary>Generate a list of entities it's colliding with.</summary>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
	<param name="rect" default="-1" type="n">Collision Rectangle your wish to check. -1 for all.</param>
	<param name="type" default="-1" type="n">Not Used.</param>
	<return>True if a collisions exist.</return>
	<example>
	if ( CollisionCalculate() )
	{
		new current;
		new angle;
		new dist;
		new rect;
		new _STATE:type;
		while ( CollisionGetCurrent(SELF, current, angle, dist, rect, type) )
		{
			if ( type == TYPE_AWAKING )
			{
				EntityPublicFunction(current, "Awaking", "sn", selfid, rect);
			}
			else if ( type == TYPE_ENEMY || type == TYPE_DOOR )
			{
				EntityPublicFunction(current, "Hit", "snnnnnn", selfid, angle, dist, APLAYER, 0, dx,dy, rect);
			}
			else if ( type == TYPE_TRANSPORT )
			{
				if ( EntityPublicFunction(current, "MovePlayer", "sn", selfid, _dir_) )
				{
					_state_ = STANDING;
				}
			}
			else if ( type == TYPE_PUSHABLE &amp;&amp; _state_ == PUSHING )
			{
				EntityPublicFunction(current, "Push", "snn", selfid, rect, angle);
			}
			else if ( type == TYPE_SWITCH )
			{
				EntityPublicFunction(current, "Pressed", "s", selfid);
			}
			else if ( type == TYPE_ITEM )
			{
				if ( EntityPublicFunction(current, "Pickup", "s", selfid) )
				{
					_state_ = LIFTING;
				}
			}
		}
	}
	</example>
</function>
<function name="CollisionGetCurrent">
	<summary>Retrieves current collision from generated list.</summary>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
	<param name="&amp;current" default="" type="n">String to store colliding entity ID.</param>
	<param name="&amp;angle" default="" type="n">Variable to store angle.</param>
	<param name="&amp;dist" default="" type="n">Variable to store distance. </param>
	<param name="&amp;rect" default="" type="n">Variable to store rectangle that being colliding with.</param>
	<param name="&amp;type" default="" type="n">Variable to store type of rectangle that your colliding with.</param>
	<return>True if a collision exist.</return>
	<example>
	if ( CollisionCalculate() )
	{
		new current;
		new angle;
		new dist;
		new rect;
		new _STATE:type;
		while ( CollisionGetCurrent(SELF, current, angle, dist, rect, type) )
		{
			if ( type == TYPE_AWAKING )
			{
				EntityPublicFunction(current, "Awaking", "sn", selfid, rect);
			}
			else if ( type == TYPE_ENEMY || type == TYPE_DOOR )
			{
				EntityPublicFunction(current, "Hit", "snnnnnn", selfid, angle, dist, APLAYER, 0, dx,dy, rect);
			}
			else if ( type == TYPE_TRANSPORT )
			{
				if ( EntityPublicFunction(current, "MovePlayer", "sn", selfid, _dir_) )
				{
					_state_ = STANDING;
				}
			}
			else if ( type == TYPE_PUSHABLE &amp;&amp; _state_ == PUSHING )
			{
				EntityPublicFunction(current, "Push", "snn", selfid, rect, angle);
			}
			else if ( type == TYPE_SWITCH )
			{
				EntityPublicFunction(current, "Pressed", "s", selfid);
			}
			else if ( type == TYPE_ITEM )
			{
				if ( EntityPublicFunction(current, "Pickup", "s", selfid) )
				{
					_state_ = LIFTING;
				}
			}
		}
	}
	</example>
</function>
<function name="CollisionGet">
	<summary>Retrieves the entity collision rectangle values.</summary>
	<param name="id" default="_" type="n">Entity ID Hash.</param>
	<param name="rect" default="" type="n">Collision Rectangle.</param>
	<param name="&amp;x" default="" type="n">Rectangle points.</param>
	<param name="&amp;y" default="" type="n">Rectangle points.</param>
	<param name="&amp;w" default="" type="n">Rectangle points.</param>
	<param name="&amp;h" default="" type="n">Rectangle points.</param>
</function>
***/
native CollisionFromObject(obj, type = 0, id = SELF);
native CollisionCheck(id1, id2, rect1 = -1, rect2 = -1);
native CollisionSet(id = SELF, rect = 0, type = 0, x = 0, y = 0, w = 0, h = 0);
native CollisionGet(id = SELF, rect, &x, &y, &w, &h);
native CollisionCalculate(id = SELF, rect = -1, type = -1);
native CollisionGetCurrent(id = SELF, &current, &angle, &dist, &rect, &type );

//Map Collision Functions
/***

***/
native MapCollisionList();
native MapCollisionCurrent(objecta[], objectb[], &angle, &dist, &rect, length_a = sizeof objecta, length_b = sizeof objectb);

///Entity Management Functions
/***
<function name="EntitiesList">
	<summary>Generate a list of entities for required map.</summary>
	<param name="map" default="CURRENT_MAP" type="n">map id you wish to check.</param>
	<return>Amount of entities on the list.</return>
</function>
<function name="EntitiesNext">
	<summary>Retrieves current entity name from generated list.</summary>
	<param name="&amp;id" default="" type="a">String to store the Entity ID.</param>
	<param name="map_id" default="CURRENT_MAP" type="n">map id you wish to check.</param>
	<param name="string[]" default="&quot;&quot;" type="a">String to store the Entity Name.</param>
	<return>True if there still more entities. False if no more is in the list.</return>
</function>
***/
native EntitiesList(mapid = 0);
native EntitiesNext(&id, map_id = 0, string{}="", maxlength = sizeof(string));


#define __MAP_ENTITY__ EntityHash("__map__");
#define __MAIN_ENTITY__ EntityHash("main");

