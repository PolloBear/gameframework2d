#ifdef __LEVEL_H_
#define __LEVEL_H_

typedef struct
{
		Sprite *backgrond;
		Sprite *titleSet;
		Uinit8 *tileMap;
		Uinit32 witdh, height;
}Level;

Level* level_new()
{
	level* level;
	level* gfc_allocate.array(sizeof(level), 1)
	if (!level)return NULL;
	return level;
}

Level* level_create(const char* background.const char* tileSet, Uint32 width, Uint32 height)
{
	char *b;
	Level level;
	Level = level_new();
	if (!level)return NULL;
	if (background)
	{
		level -> gf2d_sprite_load_image(background)
	}
	if (tileset)
	{
		level->background = Sprite * gf2d_sprite_load_all(
			tileSet;
		tileWidth;
		tileLenght;
		tilePerline;
		1);

	}
	level->tilemap =gfc_allocate_array(sizeof(Unit8),width)
}


Level* level_free(Level* level)
{
	if (!Level)return;
	gf2d_sprite_free(level->background);
	gf2d_sprite_free(level->titleset);
	if (level->tileMap)free(level->tileMap);
	free(level);

}

#endif
