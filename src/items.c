#include "items.h"
#include "simple_logger.h"
#include "world.h"
#include "player.h" 


void item_free(Entity* self) {
    if (!self) return;
    if (self->data) {
        free(self->data);
        self->data = NULL;
    }
    memset(self, 0, sizeof(Entity)); 
}

void item_think(Entity* self) {
    if (!self || !self->data) return;

    ItemBrain* brain = (ItemBrain*)self->data;
    Entity* player = brain->player;

    GFC_Vector2D dir;
    float distance;
    World* world;

    gfc_vector2d_sub(dir, player->position, self->position);
    distance = gfc_vector2d_magnitude(dir);

    if (distance < 32.0f) {

        switch (brain->type) {
        case ITEM_HEALTH:
            slog("Picked up Health (+25 HP)");
            player_heal(player, 25);
            break;
        case ITEM_AMMO:
            slog("Picked up ammo (+10 Ammo)");
            player_give_ammo(player, 10);
            break;
        case ITEM_KEYCARD:
            slog("Picked up a Keycard (Door Unlocked)");
            world = player_get_world(player);
            if (world) {
                world_open_door_by_id(world, 4);
            }
            break;
        case ITEM_SCORE:
            slog("Picked up Points (+100 Score)");
            player_add_score(player, 100);
            break;
        case ITEM_COIN:
            slog("Picked up a Coin (+1 Time Currency)");
            player_add_coin(player, 1);
            break;
        }
        entity_free(self);
    }
}
Entity* spawn_generic_item(GFC_Vector2D position, Entity* player, ItemType type, const char* image_path) {
    Entity* self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all(image_path, 32, 32, 1, 0); 
    self->position = position;
    self->hitbox = gfc_shape_rect(0, 0, 32, 32);

    ItemBrain* brain = (ItemBrain*)malloc(sizeof(ItemBrain));
    brain->player = player;
    brain->type = type;

    self->data = brain;
    self->think = item_think;
    self->free = item_free;

    return self;
}

Entity* item_health_new(GFC_Vector2D position, Entity* player) {
    return spawn_generic_item(position, player, ITEM_HEALTH, "images/pointer.png");
}

Entity* item_ammo_new(GFC_Vector2D position, Entity* player) {
    return spawn_generic_item(position, player, ITEM_AMMO, "images/pointer.png");
}

Entity* item_keycard_new(GFC_Vector2D position, Entity* player) {
    return spawn_generic_item(position, player, ITEM_KEYCARD, "images/pointer.png");
}
 
Entity* item_score(GFC_Vector2D position, Entity* player) {
    return spawn_generic_item(position, player, ITEM_SCORE, "images/pointer.png");
}

Entity* item_coin(GFC_Vector2D position, Entity* player) {
    return spawn_generic_item(position, player, ITEM_COIN, "images/pointer.png");
}