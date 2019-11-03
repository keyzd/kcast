#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "kcast.h"

void ThreeD_refresh()
{
	clear_screen();
	wall_refresh();
}

void clear_screen()
{
	SDL_Rect screen_rect1, screen_rect2;

	screen_rect1.x = 0;
	screen_rect1.y = 0;
	screen_rect1.w = win_w;
	screen_rect1.h = win_h/2;

	screen_rect2.x = 0;
	screen_rect2.y = win_h/2;
	screen_rect2.w = win_w;
	screen_rect2.h = win_h/2;

	SDL_SetRenderDrawColor(sdl_rend, 56, 56, 56, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect1);

	SDL_SetRenderDrawColor(sdl_rend, 113, 113, 113, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect2);

	/*
	SDL_Rect screen_rect;
	screen_rect.x = 0;
	screen_rect.y = 0;
	screen_rect.w = win_w;
	screen_rect.h = win_h;
	SDL_SetRenderDrawColor(sdl_rend, 0, 0, 0, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect);
	*/
}

void wall_refresh()
{
	float angle, angle_step, halffov;
	int column_x;
	int ray_len, column_len;
	uint32_t column_col;
	int side;
	int wall_i;

	halffov = player.fov / 2;
	angle_step = player.fov / win_w;
	angle = player.view_angle + halffov;

	for(column_x = 0; column_x < win_w; column_x++)
	{
		ray_len = raycast(&wall_i, &side, &column_col, angle);
		column_len = get_column_len(angle, ray_len, &column_col);
		draw_column(wall_i, side, column_len, column_col, column_x);
		angle -= angle_step;
	}
}

int raycast(
		int *wall_i,
		int *side,
		uint32_t *column_col,
		float angle)
{
	uint32_t color_vert, color_horiz; 
	int side_vert, side_horiz;
	int wall_i_vert, wall_i_horiz;

	float len_vert =
		verticalgrid_intersection(&wall_i_vert, &side_vert,
									&color_vert, angle);
	float len_horiz =
		horizontalgrid_intersection(&wall_i_horiz, &side_horiz,
									&color_horiz, angle);

	/*
	intersect_draw(512, 512, PACK_COLOR(255, 255, 255));
	*/

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

float horizontalgrid_intersection(
		int *wall_i,
		int *side,
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
	if(abs((int)angle) == 180 || abs((int)angle) == 360 || abs((int)angle) == 0)
		return 1000000.0;

	/* Finding first horiz. intersection (point A) */
	if(angle > 0 && angle < 180)
		Ay = (player.y/map.block)*map.block-1;
	else
		Ay = (player.y/map.block)*map.block+map.block;

	Ax = player.x+(player.y-Ay)/tanf(DEG2RAD(angle));

	/* Finding deltaX and deltaY */
	deltaX = map.block/tanf(DEG2RAD(angle));
	if(angle > 180 && angle < 270)
		deltaX = 0 - deltaX;
	if(angle > 270)
		deltaX = abs(deltaX);

	deltaY = map.block;
	if(angle > 0 && angle < 180)
		deltaY = 0 - deltaY;

	hit = 0;

	currentX = Ax;
	currentY = Ay;

	/* TODO: separate this loop into function */
	while(!hit)
	{
		currentGridX = currentX / map.block;
		currentGridY = currentY / map.block;

		/*
		intersect_draw(512, 512, currentX, currentY, PACK_COLOR(0, 128, 0));
		*/
		
		if(map.grid[currentGridX+currentGridY*map.grid_w] != ' ')
		{ 
			/*
			intersect_draw(512, 512, currentX, currentY, PACK_COLOR(0, 255, 0));
			*/

			hit = 1;
		}
		else
		{
			currentX += deltaX;
			currentY += deltaY;
		}

	}

	/* TODO: separate this block into function */
	switch(map.grid[currentGridX+currentGridY*map.grid_w])
	{
		case '1':
			*column_col = PACK_COLOR(192, 0, 0);
			*wall_i = '1';
			break;

		case '2':
			*column_col = PACK_COLOR(0, 192, 0);
			*wall_i = '2';
			break;

		case '3':
			*column_col = PACK_COLOR(0, 0, 192);
			*wall_i = '3';
			break;

		case '4':
			*column_col = PACK_COLOR(192, 192, 192);
			*wall_i = '4';
			break;

		default:
			*column_col = PACK_COLOR(255, 255, 0);
			*wall_i = '1';
			break;
	}

	rayX = abs(player.x - currentX);
	rayY = abs(player.y - currentY);
	rayLen = sqrt(rayX*rayX+rayY*rayY);

	*side = abs(currentX % map.block);
	return rayLen;
}

float verticalgrid_intersection(
		int *wall_i,
		int *side,
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
	if(abs((int)angle) == 90 || abs((int)angle) == 270)
		return 1000000.0;

	/* Finding first horiz. intersection (point B) */
	if(angle < 90 || angle > 270)
		Bx = (player.x/map.block)*map.block+map.block;
	else
		Bx = (player.x/map.block)*map.block-1;

	By = player.y+(player.x-Bx)*tanf(DEG2RAD(angle));

	/* Finding deltaX and deltaY */
	if(angle < 90 || angle > 270)
		deltaX = map.block;
	else
		deltaX = 0 - map.block;

	deltaY = fabsf(map.block*tanf(DEG2RAD(angle)));
	if(angle > 0 && angle < 180)
		deltaY = 0 - deltaY;

	hit = 0;

	currentX = Bx;
	currentY = By;

	/* TODO: separate this loop into function */
	while(!hit)
	{
		currentGridX = currentX / map.block;
		currentGridY = currentY / map.block;

		/*
		intersect_draw(512, 512, currentX, currentY, PACK_COLOR(128, 0, 0));
		*/

		if(map.grid[currentGridX+currentGridY*map.grid_w] != ' ')
		{
			/*
			intersect_draw(512, 512, currentX, currentY, PACK_COLOR(255, 0, 0));
			*/

			hit = 1;
		}
		else
		{
			currentX += deltaX;
			currentY += deltaY;
		}

	}

	/* TODO: separate this block into function */
	switch(map.grid[currentGridX+currentGridY*map.grid_w])
	{
		case '1':
			*column_col = PACK_COLOR(192, 0, 0);
			*wall_i = '1';
			break;

		case '2':
			*column_col = PACK_COLOR(0, 192, 0);
			*wall_i = '2';
			break;

		case '3':
			*column_col = PACK_COLOR(0, 0, 192);
			*wall_i = '3';
			break;

		case '4':
			*column_col = PACK_COLOR(192, 192, 192);
			*wall_i = '4';
			break;

		default:
			*column_col = PACK_COLOR(255, 255, 0);
			*wall_i = '1';
			break;
	}

	rayX = abs(player.x - currentX);
	rayY = abs(player.y - currentY);
	rayLen = sqrt(rayX*rayX+rayY*rayY);

	*side = abs(currentY % map.block);

	return rayLen;
}

int get_column_len(float angle, int ray_len, uint32_t *column_col)
{
	float len;
	//ray_len *= cos(DEG2RAD((int)(angle)));
	/*
	if(ray_len <= 0)
		ray_len = 1;
	*/
	len = map.block * win_h / ray_len;
	return (int)len;
}

void draw_column(
		char wall,
		int side,
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
	wall_rect_src.h = map.block;
	wall_rect_src.w = 1;
	
	if(debug)
	{
		SDL_SetRenderDrawColor(sdl_rend, r, g, b, 255);
		SDL_RenderDrawLine(sdl_rend, column_x, y_start, column_x, y_end);
	}
	else
	{
		wall_text = maptext_find(wall);
		SDL_RenderCopy(sdl_rend, wall_text, &wall_rect_src, &wall_rect_dst);
	}
}
