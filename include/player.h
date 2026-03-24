#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "world.h"

 /**
 *@breif spawn a player 
 * @return NULL on error, or a pointer to the player otherwise 
 */
Entity* player_new();

void player_set_world(Entity* player, World* world);

void player_damage(Entity* player, int damage);

void player_heal(Entity* player, int amount);

World* player_get_world(Entity* player);

int player_get_hp(Entity* player);

int player_get_max_hp(Entity* player);

Entity* player_bullet_new(GFC_Vector2D position, GFC_Vector2D velocity, World* world);

void player_give_ammo(Entity* player, int amount);

int player_get_ammo(Entity* player);

void player_add_score(Entity* player, int amount);

void player_add_coin(Entity* player, int amount);

#endif