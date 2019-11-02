#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "kcast.h"

void kc_3d_refresh(
		kc_maptext_t *mt,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	kc_clear_screen(win_w, win_h, sdl_rend, kc_map);
	kc_wall_refresh(mt, win_w, win_h, sdl_rend, kc_map, kc_player);
}

void kc_clear_screen(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map)
{
	SDL_Rect screen_rect1, screen_rect2;

	/*
	screen_rect1.x = 0;
	screen_rect1.y = 0;
	screen_rect1.w = win_w;
	screen_rect1.h = win_h/2;

	screen_rect2.x = 0;
	screen_rect2.y = win_h/2;
	screen_rect2.w = win_w;
	screen_rect2.h = win_h/2;

	SDL_SetRenderDrawColor(sdl_rend, 64, 64, 64, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect1);

	SDL_SetRenderDrawColor(sdl_rend, 96, 96, 96, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect2);
	*/

	screen_rect1.x = 0;
	screen_rect1.y = 0;
	screen_rect1.w = win_w;
	screen_rect1.h = win_h;
	SDL_SetRenderDrawColor(sdl_rend, 0, 0, 0, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect1);
}

void kc_wall_refresh(
		kc_maptext_t *mt,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	float angle, angle_step, halffov;
	int column_x;
	int ray_len, column_len;
	uint32_t column_col;
	int side;
	int wall_i;

	halffov = kc_player->fov / 2;
	angle_step = kc_player->fov / win_w;
	angle = kc_player->view_angle + halffov;

	for(column_x = 0; column_x < win_w; column_x++)
	{
		ray_len = kc_raycast(&wall_i, &side, sdl_rend, kc_map,
						kc_player, &column_col, angle);
		column_len = kc_get_column_len(kc_map, win_w, win_h,sdl_rend,
						kc_player, angle, ray_len, &column_col);
		kc_draw_column(mt, wall_i, side, win_w, win_h, sdl_rend,
			column_len, column_col, column_x);
		angle -= angle_step;
	}
}

int kc_raycast(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle)
{
	uint32_t color_vert, color_horiz; 
	int side_vert, side_horiz;
	int wall_i_vert, wall_i_horiz;

	float len_vert =
		kc_verticalgrid_intersection(&wall_i_vert, &side_vert, sdl_rend,
									 kc_map, kc_player, &color_vert, angle);
	float len_horiz =
		kc_horizontalgrid_intersection(&wall_i_horiz, &side_horiz, sdl_rend,
									   kc_map, kc_player, &color_horiz, angle);

	kc_intersect_draw(
		512, 512,
		sdl_rend,
		kc_player->unit_x, kc_player->unit_y,
		KC_PACK_COLOR(255, 255, 255));

	if(len_vert < len_horiz)
	{
		*column_col = color_vert;
		*side = side_vert;
		*wall_i = wall_i_vert;
		return (int)len_vert;
	}
	else
	{
		*column_col = color_horiz;
		*side = side_horiz;
		*wall_i = wall_i_horiz;
		return (int)len_horiz;
	}
}

float kc_horizontalgrid_intersection(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map, 
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle)
{
	int Ax, Ay; /* Point of the 1st intersection */
	int deltaX, deltaY; /* Steps for raycasting */
	int currentX, currentY, currentGridX, currentGridY;
	int rayX, rayY;
	float rayLen;
	int hit;
	int prevX, prevY;
	int mapH, mapW;

	mapH = kc_map->block * kc_map->grid_h;
	mapW = kc_map->block * kc_map->grid_w;

	/* There's no horizontal block sides */
	if(abs((int)angle) == 180 || abs((int)angle) == 360 || abs((int)angle) == 0)
		return 1000000.0;

	/* Finding first horiz. intersection (point A) */
	if(angle > 0 && angle < 180)
		Ay = (kc_player->unit_y/kc_map->block)*kc_map->block-1;
	else
		Ay = (kc_player->unit_y/kc_map->block)*kc_map->block+kc_map->block;

	Ax = kc_player->unit_x+(kc_player->unit_y-Ay)/tanf(DEG2RAD(angle));

	/* Finding deltaX and deltaY */
	deltaX = kc_map->block/tanf(DEG2RAD(angle));
	if(angle > 180 && angle < 270)
		deltaX = 0 - deltaX;
	if(angle > 270)
		deltaX = abs(deltaX);

	deltaY = kc_map->block;
	if(angle > 0 && angle < 180)
		deltaY = 0 - deltaY;

	hit = 0;

	currentX = Ax;
	currentY = Ay;

	while(!hit)
	{
		currentGridX = currentX / kc_map->block;
		currentGridY = currentY / kc_map->block;

		kc_intersect_draw(
			512, 512,
			sdl_rend,
			currentX, currentY,
			KC_PACK_COLOR(0, 128, 0));
		
		if(kc_map->grid[currentGridX+currentGridY*kc_map->grid_w] != ' ')
		{ 
			kc_intersect_draw(
				512, 512,
				sdl_rend,
				currentX, currentY,
				KC_PACK_COLOR(0, 255, 0));

			hit = 1;
		}
		else
		{
			prevX = currentX;
			prevY = currentY;
			currentX += deltaX;
			currentY += deltaY;
		}

	}

	switch(kc_map->grid[currentGridX+currentGridY*kc_map->grid_w])
	{
		case '1':
			*column_col = KC_PACK_COLOR(192, 0, 0);
			*wall_i = '1';
			break;

		case '2':
			*column_col = KC_PACK_COLOR(0, 192, 0);
			*wall_i = '2';
			break;

		case '3':
			*column_col = KC_PACK_COLOR(0, 0, 192);
			*wall_i = '3';
			break;

		case '4':
			*column_col = KC_PACK_COLOR(192, 192, 192);
			*wall_i = '4';
			break;

		default:
			*column_col = KC_PACK_COLOR(255, 255, 0);
			*wall_i = '1';
			break;
	}

	rayX = abs(kc_player->unit_x - currentX);
	rayY = abs(kc_player->unit_y - currentY);
	rayLen = sqrt(rayX*rayX+rayY*rayY);

	*side = abs(currentX % kc_map->block);
	return rayLen;
}

float kc_verticalgrid_intersection(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		kc_map_t* kc_map,
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle)
{
	int Bx, By; /* Point of the 1st intersection */
	int deltaX, deltaY; /* Steps for raycasting */
	int currentX, currentY, currentGridX, currentGridY;
	int rayX, rayY;
	float rayLen;
	int hit;
	int prevX, prevY;
	int mapH, mapW;

	mapH = kc_map->block * kc_map->grid_h;
	mapW = kc_map->block * kc_map->grid_w;

	/* There's no vertical block sides */
	if(abs((int)angle) == 90 || abs((int)angle) == 270)
		return 1000000.0;

	/* Finding first horiz. intersection (point B) */
	if(angle < 90 || angle > 270)
		Bx = (kc_player->unit_x/kc_map->block)*kc_map->block+kc_map->block;
	else
		Bx = (kc_player->unit_x/kc_map->block)*kc_map->block-1;

	By = kc_player->unit_y+(kc_player->unit_x-Bx)*tanf(DEG2RAD(angle));

	/* Finding deltaX and deltaY */
	if(angle < 90 || angle > 270)
		deltaX = kc_map->block;
	else
		deltaX = 0 - kc_map->block;

	deltaY = fabsf(kc_map->block*tanf(DEG2RAD(angle)));
	if(angle > 0 && angle < 180)
		deltaY = 0 - deltaY;

	hit = 0;

	currentX = Bx;
	currentY = By;

	while(!hit)
	{
		currentGridX = currentX / kc_map->block;
		currentGridY = currentY / kc_map->block;

		kc_intersect_draw(
			512, 512,
			sdl_rend,
			currentX, currentY,
			KC_PACK_COLOR(128, 0, 0));

		if(kc_map->grid[currentGridX+currentGridY*kc_map->grid_w] != ' ')
		{
			kc_intersect_draw(
				512, 512,
				sdl_rend,
				currentX, currentY,
				KC_PACK_COLOR(255, 0, 0));

			hit = 1;
		}
		else
		{
			prevX = currentX;
			prevY = currentY;
			currentX += deltaX;
			currentY += deltaY;
		}

	}

	switch(kc_map->grid[currentGridX+currentGridY*kc_map->grid_w])
	{
		case '1':
			*column_col = KC_PACK_COLOR(192, 0, 0);
			*wall_i = '1';
			break;

		case '2':
			*column_col = KC_PACK_COLOR(0, 192, 0);
			*wall_i = '2';
			break;

		case '3':
			*column_col = KC_PACK_COLOR(0, 0, 192);
			*wall_i = '3';
			break;

		case '4':
			*column_col = KC_PACK_COLOR(192, 192, 192);
			*wall_i = '4';
			break;

		default:
			*column_col = KC_PACK_COLOR(255, 255, 0);
			*wall_i = '1';
			break;
	}

	rayX = abs(kc_player->unit_x - currentX);
	rayY = abs(kc_player->unit_y - currentY);
	rayLen = sqrt(rayX*rayX+rayY*rayY);

	*side = abs(currentY % kc_map->block);

	return rayLen;
}

int kc_get_column_len(
		kc_map_t *kc_map,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend, 
		kc_player_t *kc_player,
		float angle,
		int ray_len,
		uint32_t *column_col)
{
	float len;
	ray_len *= cos(DEG2RAD((int)(angle)));
	if(ray_len <= 0)
		ray_len = 1;
	len = kc_map->block * win_h / ray_len;
	return (int)len;
}

void kc_draw_column(
		kc_maptext_t *mt,
		char wall,
		int side,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		int column_len,
		uint32_t column_col,
		int column_x)
{
	int y_up, y_down;
	uint8_t r, g, b;
	int y_start, y_end;
	SDL_Rect wall_rect_dst, wall_rect_src;
	SDL_Texture *wall_text;

	r = (column_col >> 16) & 255;
	g = (column_col >> 8) & 255;
	b = column_col & 255;

	if(column_len >= win_h)
		column_len = win_h - 1;

	y_up = win_h / 2 - 1;
	y_down = win_h / 2;

	y_start = y_up - column_len / 2;
	y_end = y_down + column_len / 2;

	wall_rect_dst.h = column_len;
	wall_rect_dst.x = column_x;
	wall_rect_dst.y = y_start;
	wall_rect_dst.w = 1;

	wall_rect_src.x = side;
	wall_rect_src.y = 0;
	wall_rect_src.h = 64;
	wall_rect_src.w = 1;
	
	if(kc_debug)
	{
		SDL_SetRenderDrawColor(sdl_rend, r, g, b, 255);
		SDL_RenderDrawLine(sdl_rend, column_x, y_start, column_x, y_end);
	}
	else
	{
		wall_text = kc_maptext_find(mt, wall);
		SDL_RenderCopy(sdl_rend, wall_text, &wall_rect_src, &wall_rect_dst);
	}
}
