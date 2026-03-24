#ifndef __ITEMS_H__
#define __ITEMS_H__

#include "entity.h"



typedef enum {
    ITEM_HEALTH,
    ITEM_AMMO,
    ITEM_KEYCARD,
    ITEM_DATA_DRIVE,
    ITEM_COIN,
    ITEM_SCORE,
} ItemType;


typedef struct {
    Entity* player;
    ItemType type;
} ItemBrain;


Entity* item_health_new(GFC_Vector2D position, Entity* player);
Entity* item_ammo_new(GFC_Vector2D position, Entity* player);
Entity* item_keycard_new(GFC_Vector2D position, Entity* player);
Entity* item_score(GFC_Vector2D position, Entity* player);
Entity* item_coin(GFC_Vector2D position, Entity* player);

#endif