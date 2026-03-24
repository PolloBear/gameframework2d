#ifndef __MONSTERS_H__
#define __MONSTERS_H__

#include "entity.h"
#include "world.h"
#include "player.h"


typedef struct {
    Entity* target;
    int state;
    int timer;
} JumperBrain;

typedef struct {
    Entity* target;
    int cooldown_timer;
} SentryBrain;

/**
 * @brief Spawns a basic Patroller that moves left and right
 */
Entity* monster_patroller_new(GFC_Vector2D position, World* world);

/**
 * @brief Spawns a Chaser that follows a target (the player)
 * @param target The entity to follow
 */
Entity* monster_chaser_new(GFC_Vector2D position, Entity* target);


Entity* monster_jumper_new(GFC_Vector2D position, Entity* target);



Entity* monster_sentry_new(GFC_Vector2D position, Entity* target);

Entity* monster_ghost_new(GFC_Vector2D position, Entity* target);


#endif