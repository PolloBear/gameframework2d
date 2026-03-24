#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"
#include "gfc_shape.h"
#include "SDL_rect.h"

typedef struct Entity_S
{
	Uint8		_inuse;						/** <this is the flag for keeping track of memory usage */
	Sprite		*sprite;						/** <the entity's graphic if it as once */
	float		frame;						/** <CURRENT FRAME OF ANIMATION*/
	int			team;						//Figure out who enemy and lpayer 
	GFC_Vector2D	position;					/** <where on the screen to draw the thing*/
	GFC_Vector2D	velocity;
	GFC_Shape		hitbox;
	void (*think)(struct Entity_S *self);	/** <Function to call to make decisions*/
	void (*update)(struct Entity_S *self); /** <Function to call to execute those decisions */
	void (*free)(struct Entity_S *self);	/** <clean up any custom allocated data*/
	void *data;								/** <for ad hoc addition data for the entity*/
}Entity;

/**
* @brief this initializes the enitys management system and quese upp cleanup on exit 
* @param max the maximum number of enitites that can exit at the same time 
*/
void entity_system_initialize(Uint32 max);


/**
* @brief clean up all active entities 
* @param ignore do not clean up this entity  
*/

void entity_clear_all(Entity *ignore);

/**
* @brief get a blank entity for use 
* @return NULL on no more room or error, a blank enitity otherwise 
*/

Entity *entity_new();

/**
* @brief clean up an entity, and free ots spot for futre use.
* @param self the entity to free 
*/

void entity_free(Entity *self);
/**
* @brief  run the think function for all active entities
*/

void entity_system_think();

/**
* @brief  run the update function for all active entities
*/

void entity_system_update();

/**
* @brief draw all active entities
*/

void entity_system_draw();

/**
 * @brief Scans all active entities to see if a projectile hit a specific team
 */
Entity* entity_get_enemy_collision(Entity* projectile, int target_team);

#endif
