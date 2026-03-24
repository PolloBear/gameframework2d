#include "simple_logger.h"

#include "player.h"
#include "camera.h"
#include "world.h"

typedef struct {
	World* world;
	int hp;
	int max_hp;
	int ammo;
	int invuln_timer;
	int shoot_cooldown;
	int coin;      
	int score;
	int jumps_left;     
	int jump_cooldown;
	GFC_Vector2D camera_target;
} PlayerData;

void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);

Entity* player_new()
{
	Entity* self;

	self = entity_new();
	if (!self)
	{
		slog(" failed to spawn a player entity");
		return NULL;

	}

	self->sprite = gf2d_sprite_load_all(
		"images/ed210.png",
		128,
		128,
		16,
		0);
	self->frame = 0;
	self->position = gfc_vector2d(400, 600);
	self->hitbox = gfc_shape_rect(0, 0, 64, 64);
	
	PlayerData* data = (PlayerData*)malloc(sizeof(PlayerData));
	data->world = NULL; 
	data->max_hp = 100;
	data->hp = 100;
	data->ammo = 10;
	data->shoot_cooldown = 0;
	self->data = data;
	data->coin = 0;
	data->score = 0;
	data->jumps_left = 2;  
	data->jump_cooldown = 0;


	self->think = player_think;
	self->update = player_update;
	self->free = player_free;
	data->invuln_timer = 0;
	return self;

}


void player_think(Entity* self)
{
	if (!self)return;
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	PlayerData* data = (PlayerData*)self->data;


	if (keys[SDL_SCANCODE_TAB]) {
		self->velocity.x = 0; 
		return;               
	}

	self->velocity.x = 0;
	int is_moving = 0;
	int direction = 1;

	if (keys[SDL_SCANCODE_A])
	{
		self->velocity.x = -3;
		is_moving = 1;
		direction = -1;
	}
	if (keys[SDL_SCANCODE_D])
	{
		self->velocity.x = 3;
		is_moving = 1;
		direction = 1;
	}

	if (keys[SDL_SCANCODE_LSHIFT] && is_moving)
	{
		slog("Dash");
		self->velocity.x = direction * 15; 
	}
	
	if (self->velocity.y == 0) {
		data->jumps_left = 2;
	}

	if (keys[SDL_SCANCODE_SPACE] && data->jumps_left > 0 && data->jump_cooldown == 0)
	{
		self->velocity.y = -8;      
		data->jumps_left--;         
		data->jump_cooldown = 30;   

	}
	if (keys[SDL_SCANCODE_S] && self->velocity.y != 0)
	{
		self->velocity.x = 0;  
		self->velocity.y = 25;  

	}


	if (keys[SDL_SCANCODE_S] && self->velocity.y != 0)
	{
		self->velocity.x = 0;
		self->velocity.y = 25;
	}


}

void player_update(Entity* self)
{
	GFC_Vector2D old_position;
	World* world;

	if (!self) return;

	old_position = self->position;

	PlayerData* data = (PlayerData*)self->data;
	world = data->world;
	
	if (data->invuln_timer > 0) {
		data->invuln_timer--;
	}

	// Cooldown countdown
	if (data->shoot_cooldown > 0) {
		data->shoot_cooldown--;
	}
	//Jump cooldown
	if (data->jump_cooldown > 0) {
		data->jump_cooldown--;
	}

	// Read the Mouse
	int mx, my;
	Uint32 mouse_state = SDL_GetMouseState(&mx, &my);

	///gun
	if ((mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) && data->shoot_cooldown == 0) {

		if (data->ammo > 0) {

			GFC_Vector2D offset = camera_get_offset();
			GFC_Vector2D mouse_world = gfc_vector2d(mx - offset.x, my - offset.y);

			GFC_Vector2D dir;
			gfc_vector2d_sub(dir, mouse_world, self->position);
			gfc_vector2d_normalize(&dir);

			GFC_Vector2D bullet_velocity = gfc_vector2d(dir.x * 15.0f, dir.y * 15.0f);
			GFC_Vector2D spawn_pos = gfc_vector2d(self->position.x + 32, self->position.y + 32);

			player_bullet_new(spawn_pos, bullet_velocity, world);

			// Consume ammo 
			data->ammo--;
			slog("Ammo remaining: %d", data->ammo);

			// Reset cooldown
			data->shoot_cooldown = 15;

		}
		else {
			slog("Out of Ammo");

			data->shoot_cooldown = 15;
		}


	}
	
	// Animation
	self->frame += 0.1;
	if (self->frame >= 16) self->frame = 0;

	// Gravity
	self->velocity.y += 0.2;

	// --- Horizontal movement + collision ---
	self->position.x += self->velocity.x;
	self->hitbox.s.r.x = self->position.x + 32;

	self->hitbox.s.r.y = self->position.y + 40 + 1;
	self->hitbox.s.r.h = 62;

	if (world_tile_colision(world, self->hitbox) > 0) {
		self->position.x = old_position.x;
		self->velocity.x = 0;
		self->hitbox.s.r.x = self->position.x + 32; 
	}

	self->hitbox.s.r.h = 64;
	self->hitbox.s.r.y = self->position.y + 40;

	// --- Vertical movement + collision ---
	self->position.y += self->velocity.y;
	self->hitbox.s.r.y = self->position.y + 40;

	self->hitbox.s.r.x = self->position.x + 32 + 1;
	self->hitbox.s.r.w = 62;

	if (world_tile_colision(world, self->hitbox) > 0) {

		if (self->velocity.y > 0) {
			self->position.y = old_position.y;
			self->hitbox.s.r.y = self->position.y + 40;

			while (world_tile_colision(world, self->hitbox) == 0) {
				self->position.y += 1;
				self->hitbox.s.r.y = self->position.y + 40;
			}
			self->position.y -= 1;
		}
		else {
			self->position.y = old_position.y;
		}

		self->velocity.y = 0;
		self->hitbox.s.r.y = self->position.y + 40;
	}

	// Restore the real width for drawing
	self->hitbox.s.r.w = 64;
	self->hitbox.s.r.x = self->position.x + 32;

	// Camera
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_TAB]) {

		if (keys[SDL_SCANCODE_W]) data->camera_target.y -= 25;
		if (keys[SDL_SCANCODE_S]) data->camera_target.y += 25;
		if (keys[SDL_SCANCODE_A]) data->camera_target.x -= 25;
		if (keys[SDL_SCANCODE_D]) data->camera_target.x += 25;


		camera_center_on(data->camera_target);
	}
	else {

		data->camera_target = gfc_vector2d(self->position.x + 32, self->position.y + 32);
		camera_center_on(data->camera_target);
	}
}

	


void player_free(Entity* self)
{
	if (!self) return;
	if (self->data) {
		free(self->data); 
		self->data = NULL;
	}
}
World* player_get_world(Entity* player) {
	if (!player || !player->data) return NULL;
	PlayerData* data = (PlayerData*)player->data;
	return data->world; 
}

void player_set_world(Entity* player, World* world) {
	if (!player || !player->data) return;
	PlayerData* data = (PlayerData*)player->data;
	data->world = world;
}

void player_heal(Entity* player, int amount) {
	if (!player || !player->data) return;
	PlayerData* data = (PlayerData*)player->data;

	data->hp += amount;
	if (data->hp > data->max_hp) data->hp = data->max_hp; // Cap at max

	slog("Healed HP: %d / %d", data->hp, data->max_hp);
}

void player_damage(Entity* player, int damage) {
	if (!player || !player->data) return;
	PlayerData* data = (PlayerData*)player->data;

	if (data->invuln_timer > 0) return;

	data->hp -= damage;
	data->invuln_timer = 60; 

	slog("Took %d damage. HP: %d / %d", damage, data->hp, data->max_hp);

	if (data->hp <= 0) {
		slog("DIED");
	}
}
int player_get_hp(Entity* player) {
	if (!player || !player->data) return 0;
	PlayerData* data = (PlayerData*)player->data;
	return data->hp;
}

int player_get_max_hp(Entity* player) {
	if (!player || !player->data) return 100;
	PlayerData* data = (PlayerData*)player->data;
	return data->max_hp;
}


/// I need more bullets///////////
void player_bullet_think(Entity* self) {
	if (!self || !self->data) return;
	World* world = (World*)self->data;

	// 1. Wall Collision
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

	//ENEMY COLLISION 
	Entity* hit_monster = entity_get_enemy_collision(self, 2);
	if (hit_monster) {
		slog("Eliminated");

		entity_free(hit_monster); 
		entity_free(self);        
		return;
	}

	// Keep flying forward
	self->position.x += self->velocity.x;
	self->position.y += self->velocity.y;
}

Entity* player_bullet_new(GFC_Vector2D position, GFC_Vector2D velocity, World* world) {
	Entity* self = entity_new();
	if (!self) return NULL;

	self->sprite = gf2d_sprite_load_all("images/pointer.png", 32, 32, 16, 0);
	self->position = position;
	self->velocity = velocity;
	self->hitbox = gfc_shape_rect(0, 0, 32, 32);

	self->data = world;
	self->think = player_bullet_think;
	return self;
}

void player_give_ammo(Entity* player, int amount) {
	if (!player || !player->data) return;
	PlayerData* data = (PlayerData*)player->data;

	data->ammo += amount;
	slog("Picked up ammo Current Ammo: %d", data->ammo);
}

int player_get_ammo(Entity* player) {
	if (!player || !player->data) return 0;
	PlayerData* data = (PlayerData*)player->data;
	return data->ammo;
}

void player_add_score(Entity* player, int amount) {
	if (!player || !player->data) return;
	PlayerData* data = (PlayerData*)player->data;

	data->score += amount;
	slog("Total Score: %d", data->score);
}

void player_add_coin(Entity* player, int amount) {
	if (!player || !player->data) return;
	PlayerData* data = (PlayerData*)player->data;

	data-> coin += amount;
	slog("Ammount of coins: %d", data->coin);
}
/*eol@eof*/