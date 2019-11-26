#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "kcast.h"

int door = 0;
int pointX;
int pointY;
int RayLen;

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

	/* Black screen for map view */
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
	int column_y;

	halffov = player.fov / 2;
	angle_step = player.fov / win_w;
	angle = (int)( player.view_angle + ( (int)halffov % 360 ) ) % 360;

	for(column_x = 0; column_x < win_w; column_x++)
	{
		ray_len = raycast(&wall_i, &side, &column_col, (int)angle);
		column_len = get_column_len(angle, ray_len, &column_col);
		draw_column(wall_i, side, column_len, column_col, column_x, angle,
			&column_y);
		RayLen = ray_len;
		draw_floor_column(pointX, pointY, column_x, column_y, angle);

		angle -= angle_step;
		if(angle < 0)
			angle = 360 - abs(angle);
	}
}

int raycast(int *wall_i, int *side, uint32_t *column_col, float angle)
{
	uint32_t color_vert, color_horiz; 
	int side_vert, side_horiz;
	int wall_i_vert, wall_i_horiz;
	int vertX, vertY, horizX, horizY;

	float len_vert =
		verticalgrid_intersection(&vertX, &vertY, &wall_i_vert, &side_vert,
									&color_vert, angle);
	float len_horiz =
		horizontalgrid_intersection(&horizX, &horizY, &wall_i_horiz,
									&side_horiz, &color_horiz, angle);

	//intersect_draw(player.x, player.y, PACK_COLOR(255, 255, 255));

	if(len_vert < len_horiz)
	{
		*column_col = color_vert;
		*side = side_vert;
		*wall_i = wall_i_vert;
		pointX = vertX;
		pointY = vertY;
		return (int)len_vert;
	}
	else
	{
		*column_col = color_horiz;
		*side = side_horiz;
		*wall_i = wall_i_horiz;
		pointX = horizX;
		pointY = horizY;
		return (int)len_horiz;
	}
}

float horizontalgrid_intersection(
		int *rayX, int *rayY,
		int *wall_i,
		int *side,
		uint32_t *column_col,
		float angle
		)
{
	int Ax, Ay; /* Point of the 1st intersection */
	int deltaX, deltaY; /* Steps for raycasting */
	int currentX, currentY, currentGridX, currentGridY;
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

		//intersect_draw(currentX, currentY, PACK_COLOR(0, 128, 0));
		
		if(map.grid[currentGridX+currentGridY*map.grid_w] != ' ' &&
			map.grid[currentGridX+currentGridY*map.grid_w] != 'x' &&
			map.grid[currentGridX+currentGridY*map.grid_w] != 'y' &&
			map.grid[currentGridX+currentGridY*map.grid_w] != 'z')
		{ 
			//intersect_draw(currentX, currentY, PACK_COLOR(0, 255, 0));
			hit = 1;

			/* DANGER! VERY KOSTYL HERE */
			if(map.grid[currentGridX+currentGridY*map.grid_w] == 'd' || door)
			{
				*rayX = abs(player.x - currentX);
				*rayY = abs(player.y - currentY);
				rayLen = sqrt((*rayX)*(*rayX)+(*rayY)*(*rayY));
				if(rayLen <= 32)
				{
					door = 1;
					hit = 0;
					currentX += deltaX;
					currentY += deltaY;
				}
			}
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
			//*column_col = PACK_COLOR(192, 0, 0);
			*wall_i = '1';
			break;

		case '2':
			//*column_col = PACK_COLOR(0, 192, 0);
			*wall_i = '2';
			break;

		case '3':
			//*column_col = PACK_COLOR(0, 0, 192);
			*wall_i = '3';
			break;

		case '4':
			//*column_col = PACK_COLOR(192, 192, 192);
			*wall_i = '4';
			break;

		case '5':
			*wall_i = '5';
			break;

		case '6':
			*wall_i = '6';
			break;

		case 'd':
			*wall_i = '7';
			break;

		default:
			//*column_col = PACK_COLOR(255, 255, 0);
			*wall_i = '1';
			break;
	}

	*rayX = abs(player.x - currentX);
	*rayY = abs(player.y - currentY);
	rayLen = sqrt((*rayX)*(*rayX)+(*rayY)*(*rayY));

	/* Fixing fisheye distortion */
	rayLen *= cos(DEG2RAD(fabsf(angle-player.view_angle)));

	if(map.grid[currentGridX+currentGridY*map.grid_w] == 'd')
	{
		rayLen += map.block / 2;
		door = 1;
	}

	*side = abs(currentX % map.block);
	return rayLen;
}

float verticalgrid_intersection(
		int *rayX, int *rayY,
		int *wall_i,
		int *side,
		uint32_t *column_col,
		float angle
		)
{
	int Bx, By; /* Point of the 1st intersection */
	int deltaX, deltaY; /* Steps for raycasting */
	int currentX, currentY, currentGridX, currentGridY;
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

		//intersect_draw(currentX, currentY, PACK_COLOR(128, 0, 0));

		if(map.grid[currentGridX+currentGridY*map.grid_w] != ' ' &&
			map.grid[currentGridX+currentGridY*map.grid_w] != 'x' &&
			map.grid[currentGridX+currentGridY*map.grid_w] != 'y' &&
			map.grid[currentGridX+currentGridY*map.grid_w] != 'z')
		{
			//intersect_draw(currentX, currentY, PACK_COLOR(255, 0, 0));
			hit = 1;

			/* DANGER! VERY KOSTYL HERE */
			if(map.grid[currentGridX+currentGridY*map.grid_w] == 'd' || door)
			{
				*rayX = abs(player.x - currentX);
				*rayY = abs(player.y - currentY);
				rayLen = sqrt((*rayX)*(*rayX)+(*rayY)*(*rayY));
				if(rayLen <= 32)
				{
					door = 1;
					hit = 0;
					currentX += deltaX;
					currentY += deltaY;
				}
			}
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
			//*column_col = PACK_COLOR(192, 0, 0);
			*wall_i = '1';
			break;

		case '2':
			//*column_col = PACK_COLOR(0, 192, 0);
			*wall_i = '2';
			break;

		case '3':
			//*column_col = PACK_COLOR(0, 0, 192);
			*wall_i = '3';
			break;

		case '4':
			//*column_col = PACK_COLOR(192, 192, 192);
			*wall_i = '4';
			break;

		case '5':
			*wall_i = '5';
			break;

		case '6':
			*wall_i = '6';
			break;

		case 'd':
			*wall_i = '7';
			break;

		default:
			//*column_col = PACK_COLOR(255, 255, 0);
			*wall_i = '1';
			break;
	}

	*rayX = abs(player.x - currentX);
	*rayY = abs(player.y - currentY);
	rayLen = sqrt((*rayX)*(*rayX)+(*rayY)*(*rayY));

	/* Fixing fisheye distortion */
	rayLen *= cos(DEG2RAD(fabsf(angle-player.view_angle)));

	if(map.grid[currentGridX+currentGridY*map.grid_w] == 'd')
	{
		rayLen += map.block / 2;
		door = 1;
	}

	*side = abs(currentY % map.block);

	return rayLen;
}

int get_column_len(float angle, int ray_len, uint32_t *column_col)
{
	float len;
	//if(ray_len <= 0) return 0;
	//len = map.block * win_h / ray_len;
	len = (float)map.block / ray_len * player.plane_dist;
	return (int)len;
}

void draw_column(
		char wall,
		int side,
		int column_len,
		uint32_t column_col,
		int column_x,
		float angle,
		int *screen_y)
{
	int y_up, y_down;
	uint8_t r, g, b;
	int y_start, y_end;
	SDL_Rect wall_rect_dst, wall_rect_src;
	SDL_Texture *wall_text;

	int line_left, line_right;

	r = (column_col >> 16) & 255;
	g = (column_col >> 8) & 255;
	b = column_col & 255;

	if(column_len >= win_h)
		column_len = win_h - 1;

	y_up = win_h / 2 - 1;
	y_down = win_h / 2;

	y_start = y_up - column_len / 2;
	y_end = y_down + column_len / 2;
	*screen_y = y_end;

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

uint32_t getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0; /* shouldn't happen, but avoids warnings */
    }
}

void draw_floor_column(int wall_x, int wall_y, int screen_x,
						int screen_y, float angle)
{
	int text_x, text_y, ARayX, ARayY;
	uint8_t r, g, b;
	uint32_t color;

	while(RayLen > 0)
	{
		//color = floor_a[text_x+text_y*64];
		color = getpixel(floor_text, text_x, text_y);
		r = (color >> 16) & 255;
		g = (color >> 8) & 255;
		b = color & 255;

		SDL_SetRenderDrawColor(sdl_rend, r, g, b, 255);
		SDL_RenderDrawPoint(sdl_rend, screen_x, screen_y);

		screen_y++;
		if(screen_y-1 == win_h) break;
	}
}
