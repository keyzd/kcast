#include <math.h>
#include <stdio.h>
#include "kcast.h"

void kc_3d_refresh(kc_screen_t *kc_screen, kc_map_t *kc_map, kc_player_t *kc_player)
{
	kc_clear_screen(kc_screen, kc_map);
	kc_wall_refresh(kc_screen, kc_map, kc_player);
}

void kc_clear_screen(kc_screen_t *kc_screen, kc_map_t *kc_map)
{
	int i;

	for(i = 0; i < kc_screen->w * kc_screen->h; i++)
	{
		kc_screen->pixels[i] = KC_PACK_COLOR(0, 0, 0);
	}
}

void kc_wall_refresh(kc_screen_t *kc_screen, kc_map_t *kc_map, kc_player_t *kc_player)
{
	float halffov;
	float current_a, start_a, end_a;
	float a_step;

	int column_x;
	int ray_len, column_len;
	uint32_t column_col;

	halffov = kc_player->fov/2;
	a_step = kc_player->fov/kc_screen->w;
	start_a = kc_player->fov + halffov;
	end_a = kc_player->fov - halffov;
	column_x = 0;

	for(current_a = start_a; current_a >= end_a; current_a -= a_step)
	{
		ray_len = kc_raycast(kc_map, kc_player, &column_col);
		column_len = kc_get_column_len(kc_screen, ray_len, &column_col);
		kc_draw_column(kc_screen, column_len, column_col, column_x);
		column_x++;
	}
}

int kc_raycast(kc_map_t* kc_map, kc_player_t *kc_player, uint32_t *column_col)
{
	return 0;
}

int kc_get_column_len(kc_screen_t *kc_screen, int ray_len, uint32_t *column_col)
{
	return 0;
}

void kc_draw_column(kc_screen_t *kc_screen, int column_len, uint32_t column_col, int column_x)
{
}
