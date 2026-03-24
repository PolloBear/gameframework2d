#include "monster.h"
#include "simple_logger.h"
#include "world.h"
#include "gfc_shape.h"

// PATROLLER LOGIC
void patroller_think(Entity* self) {
    if (!self || !self->data) return;
    World* world = (World*)self->data; 

    GFC_Shape next_hitbox;

    next_hitbox = gfc_shape_rect(
        self->position.x + self->velocity.x + self->hitbox.s.r.x,
        self->position.y + self->velocity.y + self->hitbox.s.r.y,
        self->hitbox.s.r.w,
        self->hitbox.s.r.h
    );

    if (world_tile_colision(world, next_hitbox)) {
        self->velocity.x *= -1;
    }

}

Entity* monster_patroller_new(GFC_Vector2D position, World* world) {
    Entity* self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all("images/space_bug.png", 128, 128, 16, 0);
    self->position = position;
    self->team = 2;
    self->velocity = gfc_vector2d(2, 0);

    self->hitbox = gfc_shape_rect(0, 0, 128, 128);

    self->data = world; 
    self->think = patroller_think;
    return self;
}

// CHASER LOGIC 
void chaser_think(Entity* self) {
    if (!self || !self->data) return;
    Entity* target = (Entity*)self->data;
    World* world = player_get_world(target);

    if (target) {
        float mx = self->position.x + self->hitbox.s.r.x;
        float my = self->position.y + self->hitbox.s.r.y;
        float mw = self->hitbox.s.r.w;
        float mh = self->hitbox.s.r.h;

        float px = target->hitbox.s.r.x;
        float py = target->hitbox.s.r.y;
        float pw = target->hitbox.s.r.w;
        float ph = target->hitbox.s.r.h;

        if (mx < px + pw && mx + mw > px && my < py + ph && my + mh > py) {
            player_damage(target, 10);
        }
    }

    GFC_Vector2D dir;
    gfc_vector2d_sub(dir, target->position, self->position);

    float distance = gfc_vector2d_magnitude(dir);

    if (distance > 400.0f || distance < 3.0f) {
        self->velocity.x = 0;
        self->velocity.y = 0;
        return;
    }

    gfc_vector2d_normalize(&dir);
    self->velocity.x = dir.x * 1.5f;
    self->velocity.y = dir.y * 1.5f;

    // WALL COLLISION
    if (world) {
        GFC_Shape next_hitbox;
        next_hitbox = gfc_shape_rect(
            self->position.x + self->velocity.x + self->hitbox.s.r.x,
            self->position.y + self->velocity.y + self->hitbox.s.r.y,
            self->hitbox.s.r.w,
            self->hitbox.s.r.h
        );

        if (world_tile_colision(world, next_hitbox)) {
            self->velocity.x = 0;
            self->velocity.y = 0;
        }
    }
}

Entity* monster_chaser_new(GFC_Vector2D position, Entity* target) {
    Entity* self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all("images/space_bug.png", 64, 64, 16, 0);
    self->position = position;
    self->team = 2;


    // THE HITBOX
    self->hitbox = gfc_shape_rect(0, 0, 64, 64);

    self->data = target;
    self->think = chaser_think;
    return self;
}

// JUMPER LOGIC 

void jumper_free(Entity* self) {
    if (!self) return;
    if (self->data) {
        free(self->data); 
        self->data = NULL;
    }
}

void jumper_think(Entity* self) {
    if (!self || !self->data) return;

    JumperBrain* brain = (JumperBrain*)self->data;
    Entity* target = brain->target;
    World* world = player_get_world(target);

    float gravity = 0.5f;
    self->velocity.y += gravity;

    if (target) {
        float mx = self->position.x + self->hitbox.s.r.x;
        float my = self->position.y + self->hitbox.s.r.y;
        float mw = self->hitbox.s.r.w;
        float mh = self->hitbox.s.r.h;

        float px = target->hitbox.s.r.x;
        float py = target->hitbox.s.r.y;
        float pw = target->hitbox.s.r.w;
        float ph = target->hitbox.s.r.h;

        if (mx < px + pw && mx + mw > px && my < py + ph && my + mh > py) {
            player_damage(target, 10);
        }
    }

    if (brain->state == 0) { 
        self->velocity.x *= 0.8f;

        brain->timer++;

        if (brain->timer > 120) {
            brain->state = 1;
            brain->timer = 0;

            float dir_x = target->position.x - self->position.x;
            float sign_x = (dir_x > 0) ? 1.0f : -1.0f; 

            self->velocity.y = -12.0f;         
            self->velocity.x = sign_x * 6.0f;  
        }
    }

    if (world) {
        GFC_Shape next_x = gfc_shape_rect(
            self->position.x + self->velocity.x + self->hitbox.s.r.x,
            self->position.y + self->hitbox.s.r.y, 
            self->hitbox.s.r.w,
            self->hitbox.s.r.h
        );

        if (world_tile_colision(world, next_x)) {
            self->velocity.x = 0; 
        }

        GFC_Shape next_y = gfc_shape_rect(
            self->position.x + self->hitbox.s.r.x, 
            self->position.y + self->velocity.y + self->hitbox.s.r.y,
            self->hitbox.s.r.w,
            self->hitbox.s.r.h
        );

        if (world_tile_colision(world, next_y)) {
            if (self->velocity.y > 0) {
                self->velocity.y = 0;
                brain->state = 0; 
            }
            else {
                self->velocity.y = 0;
            }
        }
    }
    if (target) {
        float mx = self->position.x + self->hitbox.s.r.x;
        float my = self->position.y + self->hitbox.s.r.y;
        float mw = self->hitbox.s.r.w;
        float mh = self->hitbox.s.r.h;

        float px = target->hitbox.s.r.x;
        float py = target->hitbox.s.r.y;
        float pw = target->hitbox.s.r.w;
        float ph = target->hitbox.s.r.h;

        if (mx < px + pw && mx + mw > px && my < py + ph && my + mh > py) {
            player_damage(target, 10);
        }
    }
}

Entity* monster_jumper_new(GFC_Vector2D position, Entity* target) {
    Entity* self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all("images/space_bug.png", 128, 128, 16, 0);
    self->position = position;
    self->team = 2;
    self->hitbox = gfc_shape_rect(0, 0, 128, 128);

    JumperBrain* brain = (JumperBrain*)malloc(sizeof(JumperBrain));
    brain->target = target;
    brain->state = 0;
    brain->timer = 0;

    self->data = brain;
    self->think = jumper_think;
    self->free = jumper_free; 

    return self;
}

void projectile_think(Entity* self) {
    if (!self || !self->data) return;
    World* world = (World*)self->data; // The bullet stores the world in its data pointer

    // Check if the bullet is about to hit a wall
    GFC_Shape next_hitbox = gfc_shape_rect(
        self->position.x + self->velocity.x + self->hitbox.s.r.x,
        self->position.y + self->velocity.y + self->hitbox.s.r.y,
        self->hitbox.s.r.w,
        self->hitbox.s.r.h
    );

    if (world_tile_colision(world, next_hitbox)) {
        entity_free(self);
        return;
    }
}

Entity* projectile_new(GFC_Vector2D position, GFC_Vector2D velocity, World* world) {
    Entity* self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all("images/space_bug.png", 32, 32, 16, 0);
    self->position = position;
    self->velocity = velocity;
    self->hitbox = gfc_shape_rect(0, 0, 32, 32);

    self->data = world; 
    self->think = projectile_think;
    return self;
}


// SENTRY LOGIC 
void sentry_free(Entity* self) {
    if (!self) return;
    if (self->data) {
        free(self->data); 
        self->data = NULL;
    }
}

void sentry_think(Entity* self) {
    if (!self || !self->data) return;

    SentryBrain* brain = (SentryBrain*)self->data;
    Entity* target = brain->target;
    World* world = player_get_world(target);

    self->velocity.x = 0;
    self->velocity.y = 0;

    // Handle the shooting cooldown
    if (brain->cooldown_timer > 0) {
        brain->cooldown_timer--; 
    }
    else {
        GFC_Vector2D dir;
        gfc_vector2d_sub(dir, target->position, self->position);
        float distance = gfc_vector2d_magnitude(dir);

        if (distance < 600.0f) {
            gfc_vector2d_normalize(&dir);

            // Set the bullet speed to 5.0
            GFC_Vector2D bullet_velocity = gfc_vector2d(dir.x * 5.0f, dir.y * 5.0f);

            GFC_Vector2D spawn_pos = gfc_vector2d(self->position.x + 48, self->position.y + 48);

            // Spawn the bullet
            projectile_new(spawn_pos, bullet_velocity, world);

            brain->cooldown_timer = 90;
        }
    }
}

Entity* monster_sentry_new(GFC_Vector2D position, Entity* target) {
    Entity* self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all("images/space_bug.png", 128, 128, 16, 0);
    self->position = position;
    self->team = 2;
    self->hitbox = gfc_shape_rect(0, 0, 128, 128);

    // Allocate the Sentry's brain
    SentryBrain* brain = (SentryBrain*)malloc(sizeof(SentryBrain));
    brain->target = target;
    brain->cooldown_timer = 60; 

    self->data = brain;
    self->think = sentry_think;
    self->free = sentry_free;

    return self;
}

// GHOST LOGIC
void ghost_think(Entity* self) {
    if (!self || !self->data) return;
    Entity* target = (Entity*)self->data;

    if (target) {
        float mx = self->position.x + self->hitbox.s.r.x;
        float my = self->position.y + self->hitbox.s.r.y;
        float mw = self->hitbox.s.r.w;
        float mh = self->hitbox.s.r.h;

        float px = target->hitbox.s.r.x;
        float py = target->hitbox.s.r.y;
        float pw = target->hitbox.s.r.w;
        float ph = target->hitbox.s.r.h;

        if (mx < px + pw && mx + mw > px && my < py + ph && my + mh > py) {
            player_damage(target, 10);
        }
    }

    GFC_Vector2D dir;
    gfc_vector2d_sub(dir, target->position, self->position);

    float distance = gfc_vector2d_magnitude(dir);
    if (distance < 3.0f) {
        self->velocity.x = 0;
        self->velocity.y = 0;
        return;
    }

    gfc_vector2d_normalize(&dir);
    self->velocity.x = dir.x * 0.8f;
    self->velocity.y = dir.y * 0.8f;
}

Entity* monster_ghost_new(GFC_Vector2D position, Entity* target) {
    Entity* self = entity_new();
    if (!self) return NULL;

    self->sprite = gf2d_sprite_load_all("images/space_bug.png", 128, 128, 16, 0);
    self->position = position;
    self->team = 2;
    self->hitbox = gfc_shape_rect(0, 0, 128, 128);

    self->data = target;
    self->think = ghost_think;

    return self;
}