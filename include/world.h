#ifndef __WORLD_H__
#define __WORLD_H__

#include "gf2d_sprite.h"

typedef struct
{
	Sprite	*background;		/**<backgrund image or the world*/
	Sprite *tileLayer;			/**<prerendered tilelayer*/
	Sprite	*tileSet;			/**<sprite containing tiles for the world*/
	Uint8	*tileMap;			/**<the tiles that make up the world*/
	Uint32 tileHeight;			/**<how many tiles tall the map is*/
	Uint32 tileWidth;			/**<how many tiles wide the map is*/
	GFC_Shape tilehitbox;
}World;


/**
*@brief load a world from a config file
*@param filename the name of the world files to load
*@return NULL on error, or a usable world otherwise
*/
 
World* world_load(const char* filename);


/** test to see the world working*/

World *world_test_new();

/**
* @breif set the camrea bounds to the world size
*/
void world_setup_camera(World* world);

/**
* @brief allocare a new empty world 
* @return NULL on error, or a blank world
*/
World *world_new();

/**
*@brief free a previously allocated world
* @param world the world to free
*/

void world_free(World *world);

/**
*@brief draw the world
*@param world the world to draw 
*/

void world_draw(World* world);


Uint8 world_get_tile_at(World* world, GFC_Vector2D position);


Uint8 world_tile_colision(World* world, GFC_Shape shape);

void world_set_tile(World* world, int tile_x, int tile_y, Uint8 tile_index);


#endif
