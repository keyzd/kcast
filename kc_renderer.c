#include <math.h>
#include <stdio.h>

#include "kcast.h"

void kc_3d_refresh(
		kc_screen_t *kc_screen,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	kc_clear_screen(kc_screen, kc_map);
	kc_wall_refresh(kc_screen, kc_map, kc_player);
}

void kc_clear_screen(
		kc_screen_t *kc_screen,
		kc_map_t *kc_map)
{
	int x;

	for(x = 0; x < kc_screen->w * kc_screen->h; x++)
	{
		kc_screen->pixels[x] = KC_PACK_COLOR(0, 0, 0);
	}
}

void kc_wall_refresh(
		kc_screen_t *kc_screen,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	float angle, angle_step, halffov;
	int column_x;
	int ray_len, column_len;
	uint32_t column_col;

	halffov = kc_player->fov / 2;
	angle_step = kc_player->fov / kc_screen->w;
	angle = kc_player->view_angle + halffov;

	for(column_x = 0; column_x < kc_screen->w; column_x++)
	{
		ray_len = kc_raycast(kc_map, kc_player, &column_col, angle);
		column_len = kc_get_column_len(kc_screen, ray_len, &column_col);
		kc_draw_column(kc_screen, column_len, column_col, column_x);
		angle -= angle_step;
	}
}

int kc_raycast(
		kc_map_t* kc_map,
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle)
{
	float len_vert = kc_verticalgrid_intersection(kc_map, kc_player, angle);
	float len_horiz = kc_horizontalgrid_intersection(kc_map, kc_player, angle);

	if(len_vert < len_horiz)
	{
		return len_vert;
	}
	else
	{
		return len_horiz;
	}
}

float kc_verticalgrid_intersection(
		kc_map_t* kc_map,
		kc_player_t *kc_player,
		float angle)
{
	return 0;
}

float kc_horizontalgrid_intersection(
		kc_map_t* kc_map, 
		kc_player_t *kc_player,
		float angle)
{
	return 0;
}

int kc_get_column_len(
		kc_screen_t *kc_screen, 
		int ray_len,
		uint32_t *column_col)
{
	return 0;
}

void kc_draw_column(
		kc_screen_t *kc_screen,
		int column_len,
		uint32_t column_col,
		int column_x)
{
	int y_up, y_down;

	y_up = kc_screen->h / 2 - 1;
	y_down = kc_screen->h / 2;

	if(column_len >= kc_screen->h)
		column_len = kc_screen->h - 1;
	
	for(; column_len > 0; column_len -= 2)
	{
		kc_screen->pixels[column_x + y_up * kc_screen->w] = column_col;
		kc_screen->pixels[column_x + y_down * kc_screen->w] = column_col;

		y_up--;
		y_down++;
	}
}
