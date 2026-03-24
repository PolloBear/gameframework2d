#include "simple_logger.h"

#include "entity.h"
#include "camera.h"

typedef struct 
{
	Entity *entity_list;
	Uint32 entity_max;

}EntityManager;

void entity_system_close();

static EntityManager _entity_manager = {0}; /**<initatlize a local global entity manager */


void entity_system_initialize(Uint32 max) 
{
    if (_entity_manager.entity_list) 
    {
        slog("cannot have two instances of entity manager");
        return;
    }
    if (!max)
    {
        slog("cannot allocate 0 entities!");
        return;
    }
    _entity_manager.entity_list = gfc_allocate_array(sizeof(Entity), max);
    if (!_entity_manager.entity_list) 
    {
        slog("failed to allocate global entity list");
        return;
    }
    _entity_manager.entity_max = max;
    atexit(entity_system_close);
}

void entity_system_close()
{
	entity_clear_all(NULL);
	if (_entity_manager.entity_list) free(_entity_manager.entity_list);
	memset(&_entity_manager, 0, sizeof(EntityManager));
}

void entity_clear_all(Entity* ignore)
{
	int i;
	for (i = 0; i < _entity_manager.entity_max; i++)
	{
		if (&_entity_manager.entity_list[i] == ignore) continue;
		if (!_entity_manager.entity_list[i]._inuse) continue; 
		entity_free(&_entity_manager.entity_list[i]);
	}

}

Entity* entity_new()
{
	int i;
	for (i = 0; i < _entity_manager.entity_max; i++)
	{
		if (_entity_manager.entity_list[i]._inuse) continue; 
		memset(&_entity_manager.entity_list[i], 0, sizeof(Entity));
		_entity_manager.entity_list[i]._inuse = 1;
		return &_entity_manager.entity_list[i];
	}
	slog("no more available entities");
	return NULL;
}



void entity_free(Entity *self) 
{
	if (!self) return; // cant do work on nothing
	if (self->sprite) gf2d_sprite_free(self->sprite);
	if (self->free) self->free(self);

	memset(self, 0, sizeof(Entity));

}

void entity_think(Entity* self)
{
	if (!self || !self->_inuse) return;
	if (self->think) self->think(self);
}

void entity_system_think()
{
	int i;
	for (i = 0; i < _entity_manager.entity_max; i++)
	{
		if (!_entity_manager.entity_list[i]._inuse) continue; 
		entity_think(&_entity_manager.entity_list[i]); 
	}
}


void entity_update(Entity* self)
{
	if (!self || !self->_inuse) return;
	
	self->frame += 0.1;
	if (self->frame >= 16)self->frame = 0;

	if (self->update) self->update(self); 
	gfc_vector2d_add(self->position, self->position, self->velocity);
}

void entity_system_update()
{
	int i;
	for (i = 0; i < _entity_manager.entity_max; i++)
	{
		if (!_entity_manager.entity_list[i]._inuse) continue;
		entity_update(&_entity_manager.entity_list[i]);
	}
}

void entity_draw(Entity* self) 
{
	GFC_Vector2D offset;
	GFC_Vector2D position ;
	if (!self)return;
	offset = camera_get_offset();
	gfc_vector2d_add(position, self->position, offset);
	if (self->sprite) 
	{
		gf2d_sprite_render(
			self->sprite,
			position,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			(Uint32)self-> frame);
	}
}

void entity_system_draw()
{
	int i;
	for (i = 0; i < _entity_manager.entity_max; i++)
	{
		if (!_entity_manager.entity_list[i]._inuse) continue; //skip any active entittes
		entity_draw(&_entity_manager.entity_list[i]);
	}
}

Entity* entity_get_enemy_collision(Entity* projectile, int target_team) {
	int i;
	Entity* other;
	for (i = 0; i < _entity_manager.entity_max; i++) {
		other = &_entity_manager.entity_list[i];

		if (!other->_inuse) continue;
		if (other == projectile) continue;
		if (other->team != target_team) continue;

		float px = projectile->position.x + projectile->hitbox.s.r.x;
		float py = projectile->position.y + projectile->hitbox.s.r.y;
		float pw = projectile->hitbox.s.r.w;
		float ph = projectile->hitbox.s.r.h;

		float ox = other->position.x + other->hitbox.s.r.x;
		float oy = other->position.y + other->hitbox.s.r.y;
		float ow = other->hitbox.s.r.w;
		float oh = other->hitbox.s.r.h;

		if (px < ox + ow && px + pw > ox && py < oy + oh && py + ph > oy) {
			return other; 
		}
	}
	return NULL; // Hit nothing
}
/*eol@eof*/