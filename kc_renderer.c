/*
	Branch: new rendering approach (via SDL2 Renderer API)
*/
#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "kcast.h"

void kc_3d_refresh(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	kc_clear_screen(win_w, win_h, sdl_rend, kc_map);
	kc_wall_refresh(win_w, win_h, sdl_rend, kc_map, kc_player);
}

void kc_clear_screen(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map)
{
	SDL_Rect screen_rect1, screen_rect2;

	screen_rect1.x = 0;
	screen_rect1.y = 0;
	screen_rect1.w = win_w;
	screen_rect1.h = win_h/2;

	screen_rect2.x = 0;
	screen_rect2.y = win_h/2+1;
	screen_rect2.w = win_w;
	screen_rect2.h = win_h/2;

	SDL_SetRenderDrawColor(sdl_rend, 64, 64, 64, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect2);

	SDL_SetRenderDrawColor(sdl_rend, 96, 96, 96, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect1);
}

void kc_wall_refresh(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	float angle, angle_step, halffov;
	int column_x;
	int ray_len, column_len;
	uint32_t column_col;

	halffov = kc_player->fov / 2;
	angle_step = kc_player->fov / win_w;
	angle = kc_player->view_angle + halffov;

	for(column_x = 0; column_x < win_w; column_x++)
	{
		ray_len = kc_raycast(sdl_rend, kc_map, kc_player,
						&column_col, (int)angle);
		column_len = kc_get_column_len(win_w, win_h,sdl_rend, kc_player, (int)angle,
								ray_len, &column_col);
		kc_draw_column(win_w, win_h, sdl_rend, column_len, column_col, column_x);
		angle -= angle_step;
	}
}

int kc_raycast(
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle)
{
	uint32_t color_vert, color_horiz; 
	float len_vert =
		kc_verticalgrid_intersection(sdl_rend, kc_map, kc_player,
									 &color_vert, angle);
	float len_horiz =
		kc_horizontalgrid_intersection(sdl_rend, kc_map, kc_player,
									   &color_horiz, angle);

	/*
	kc_intersect_draw(sdl_rend, kc_player->unit_x, kc_player->unit_y,
						KC_PACK_COLOR(255, 255, 255));
	*/

	if(len_vert < len_horiz)
	{
		*column_col = color_vert;
		//float z = len_vert*cos(DEG2RAD(angle-kc_player->view_angle));
		return (int)len_vert;
	}
	else
	{
		*column_col = color_horiz;
		//float z = len_horiz*cos(DEG2RAD(angle-kc_player->view_angle));
		return (int)len_horiz;
	}
}

float kc_horizontalgrid_intersection(
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

	/* There's no horizontal block sides */
	if((int)angle == 180 || (int)angle == 360 || (int)angle == 0)
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

		/*
		kc_intersect_draw(sdl_rend, currentX, currentY,
							KC_PACK_COLOR(0, 128, 0));
		*/

		if(kc_map->grid[currentGridX+currentGridY*kc_map->grid_w] != ' ')
		{ 
			/*
			kc_intersect_draw(sdl_rend, currentX, currentY,
								KC_PACK_COLOR(0, 255, 0));
			*/
			hit = 1;
		}
		else
		{
			currentX += deltaX;
			currentY += deltaY;
		}
	}

	switch(kc_map->grid[currentGridX+currentGridY*kc_map->grid_w])
	{
		case '1':
			*column_col = KC_PACK_COLOR(192, 0, 0);
			break;

		case '2':
			*column_col = KC_PACK_COLOR(0, 192, 0);
			break;

		case '3':
			*column_col = KC_PACK_COLOR(0, 0, 192);
			break;

		case '4':
			*column_col = KC_PACK_COLOR(192, 192, 192);
			break;

		default:
			*column_col = KC_PACK_COLOR(255, 255, 0);
			break;
	}

	rayX = abs(kc_player->unit_x - currentX);
	rayY = abs(kc_player->unit_y - currentY);
	rayLen = sqrt(rayX*rayX+rayY*rayY);

	return rayLen;
}

float kc_verticalgrid_intersection(
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

	/* There's no vertical block sides */
	if((int)angle == 90 || (int)angle == 270)
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

		/*
		kc_intersect_draw(sdl_rend, currentX, currentY,
							KC_PACK_COLOR(128, 0, 0));
		*/

		if(kc_map->grid[currentGridX+currentGridY*kc_map->grid_w] != ' ')
		{
			/*
			kc_intersect_draw(sdl_rend, currentX, currentY,
								KC_PACK_COLOR(255, 0, 0));
			*/
			hit = 1;
		}
		else
		{
			currentX += deltaX;
			currentY += deltaY;
		}
	}

	switch(kc_map->grid[currentGridX+currentGridY*kc_map->grid_w])
	{
		case '1':
			*column_col = KC_PACK_COLOR(192, 0, 0);
			break;

		case '2':
			*column_col = KC_PACK_COLOR(0, 192, 0);
			break;

		case '3':
			*column_col = KC_PACK_COLOR(0, 0, 192);
			break;

		case '4':
			*column_col = KC_PACK_COLOR(192, 192, 192);
			break;

		default:
			*column_col = KC_PACK_COLOR(255, 255, 0);
			break;
	}

	rayX = abs(kc_player->unit_x - currentX);
	rayY = abs(kc_player->unit_y - currentY);
	rayLen = sqrt(rayX*rayX+rayY*rayY);

	return rayLen;
}

int kc_get_column_len(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend, 
		kc_player_t *kc_player,
		float angle,
		int ray_len,
		uint32_t *column_col)
{
	float len;
	//len = kc_screen->h / (ray_len * cos(DEG2RAD(angle-kc_player->view_angle)));
	if(ray_len == 0)
		len = 0;
	else
		len = 24 * win_h / ray_len;
	return (int)len;
}

void kc_draw_column(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		int column_len,
		uint32_t column_col,
		int column_x)
{
	int y_up, y_down;
	uint8_t r, g, b;
	int y_start, y_end;

	if(column_len >= win_h)
		column_len = win_h - 1;

	r = (column_col >> 16) & 255;
	g = (column_col >> 8) & 255;
	b = column_col & 255;

	y_up = win_h / 2 - 1;
	y_down = win_h / 2;

	y_start = y_up - column_len / 2;
	y_end = y_down + column_len / 2;

	
	SDL_SetRenderDrawColor(sdl_rend, r, g, b, 255);
	SDL_RenderDrawLine(sdl_rend,
						column_x, y_start,
						column_x, y_end);

	/*
	for(; column_len > 0; column_len -= 2)
	{
		SDL_RenderDrawPoint(sdl_rend, column_x, y_up);
		SDL_RenderDrawPoint(sdl_rend, column_x, y_down);

		y_up--;
		y_down++;
	}
	*/
}
