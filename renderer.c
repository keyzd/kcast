#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "kcast.h"

int RayLen;
int L;

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

	//SDL_SetRenderDrawColor(sdl_rend, 56, 56, 56, 255);
	SDL_SetRenderDrawColor(sdl_rend, 0, 0, 0, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect1);

	//SDL_SetRenderDrawColor(sdl_rend, 113, 113, 113, 255);
	SDL_SetRenderDrawColor(sdl_rend, 0, 0, 0, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect2);
}

void wall_refresh()
{
	float angle, angle_step, halffov;
	int column_x;
	int ray_len, column_len;
	u32 column_col;
	int side;
	int wall_i;
	int column_y;

	halffov = player.fov / 2;
	angle_step = player.fov / win_w;
	angle = (int)( player.view_angle + ( (int)halffov % 360 ) ) % 360;

	for(column_x = 0; column_x < win_w; column_x++)
	{
		ray_len = raycast(&wall_i, &side, &column_col, angle);
		column_len = get_column_len(ray_len);
		draw_column(wall_i, side, column_len, column_col, column_x, &column_y);

		RayLen = ray_len;
		draw_floor_column(column_x, column_y, angle);
		//draw_floor_column(column_x, win_h/2, angle);

		angle -= angle_step;
		if(angle < 0)
			angle = 360.0f - fabsf(angle);
	}
}

int raycast(int *wall_i, int *side, u32 *column_col, float angle)
{
	u32 color_vert, color_horiz; 
	int side_vert, side_horiz;
	int wall_i_vert, wall_i_horiz;
	int vertX, vertY, horizX, horizY;

	float len_vert =
		verticalgrid_intersection(&vertX, &vertY, &wall_i_vert, &side_vert,
									&color_vert, angle);
	float len_horiz =
		horizontalgrid_intersection(&horizX, &horizY, &wall_i_horiz,
									&side_horiz, &color_horiz, angle);

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
		int *rayX, int *rayY,
		int *wall_i,
		int *side,
		u32 *column_col,
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
		
		char wall_char = map.grid[currentGridX+currentGridY*map.grid_w];
		if(wall_char != ' ') 
		{ 
			hit = 1;
		}
		else
		{
			currentX += deltaX;
			currentY += deltaY;
		}
	}

	/* Choose wall texture and color */
	set_wall_index(map.grid[currentGridX+currentGridY*map.grid_w],
					wall_i, column_col);

	*side = abs(currentX % map.block);

	*rayX = abs(player.x - currentX);
	*rayY = abs(player.y - currentY);
	rayLen = sqrt((*rayX)*(*rayX)+(*rayY)*(*rayY));

	/* Fixing fisheye distortion */
	rayLen *= cos(DEG2RAD(fabsf(angle-player.view_angle)));

	return rayLen;
}

float verticalgrid_intersection(
		int *rayX, int *rayY,
		int *wall_i,
		int *side,
		u32 *column_col,
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

	while(!hit)
	{
		currentGridX = currentX / map.block;
		currentGridY = currentY / map.block;

		char wall_char = map.grid[currentGridX+currentGridY*map.grid_w];
		if(wall_char != ' ')
		{
			hit = 1;
		}
		else
		{
			currentX += deltaX;
			currentY += deltaY;
		}
	}

	/* Choose wall texture and color */
	set_wall_index(map.grid[currentGridX+currentGridY*map.grid_w],
					wall_i, column_col);

	*side = abs(currentY % map.block);

	*rayX = abs(player.x - currentX);
	*rayY = abs(player.y - currentY);
	rayLen = sqrt((*rayX)*(*rayX)+(*rayY)*(*rayY));

	/* Fixing fisheye distortion */
	rayLen *= cos(DEG2RAD(fabsf(angle-player.view_angle)));

	return rayLen;
}

void set_wall_index(char wall, int *wall_i, u32 *column_col)
{
	*wall_i = wall;

	switch(wall)
	{
		case '1':
			*column_col = PACK_COLOR(181, 137, 0);
			break;

		case '2':
			*column_col = PACK_COLOR(203, 75, 22);
			break;

		case '3':
			*column_col = PACK_COLOR(220, 50, 47);
			break;

		case '4':
			*column_col = PACK_COLOR(211, 54, 130);
			break;

		case '5':
			*column_col = PACK_COLOR(108, 113, 196);
			break;

		case '6':
			*column_col = PACK_COLOR(38, 139, 210);
			break;

		case '7':
			*column_col = PACK_COLOR(42, 161, 152);
			break;

		case '8':
			*column_col = PACK_COLOR(133, 153, 0);
			break;

		default:
			*wall_i = '0';
			*column_col = PACK_COLOR(0, 0, 0);
			break;
	}
}

int get_column_len(int ray_len)
{
	float len;
	if(ray_len <= 0) return 1;
	len = map.block * win_h / ray_len;
	//len = (float)map.block / ray_len * player.plane_dist;
	return (int)len;
}

void draw_column(
		char wall,
		int side,
		int column_len,
		u32 column_col,
		int column_x,
		int *screen_y)
{
	int y_up, y_down;
	u8 r, g, b;
	int y_start, y_end;
	SDL_Rect wall_rect_dst, wall_rect_src;
	SDL_Texture *wall_text;

	r = (column_col >> 16) & 255;
	g = (column_col >> 8) & 255;
	b = column_col & 255;

	if(column_len >= win_h)
		column_len = win_h-1;

	y_up = win_h / 2 - 1;
	y_down = win_h / 2;

	y_start = y_up - column_len / 2;
	y_end = y_down + column_len / 2;
	//L = win_w/2 - column_len/2;
	L = win_w/2;
	*screen_y = y_end-1;

	wall_rect_dst.h = column_len;
	wall_rect_dst.x = column_x;
	wall_rect_dst.y = y_start;
	wall_rect_dst.w = 1;

	wall_rect_src.x = side;
	wall_rect_src.y = 0;
	wall_rect_src.h = map.block;
	wall_rect_src.w = 1;
	
	if(turnTextures)
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

void draw_floor_column(int screen_x, int screen_y, float angle)
{
	/*
	if(RayLen <= player.plane_dist)
		RayLen += 200;
	*/
	u8 r, g, b;
	u32 color;

	float alpha, beta, epsilon;
	int floorIntLen, floorIntX, floorIntY;
	int textX, textY;
	int ph = map.block/2; /* player's height */
	int HB = L/8;
	
	alpha = atanf((float)(RayLen/ph));

	alpha = RAD2DEG(alpha);
	//alpha = 89.0;
	beta = atanf((float)(player.plane_dist/ph));
	beta = RAD2DEG(beta);

	//epsilon = fabsf(alpha-beta)/HB; /* ray step */
	epsilon = fabsf(89.0-beta)/HB;

	if((int)angle == (int)player.view_angle)
	{
		/*
		printf("%d %.2f %.2f\n", RayLen, RAD2DEG(alpha), RAD2DEG(beta));
		printf("%d\n\n", L);
		*/
		//printf("%d a: %f b: %f a-b: %f\n", RayLen, alpha, beta, fabsf(alpha-beta));
	}

	floorIntLen = RayLen;

	SDL_Surface *pixels = NULL;

	for(; screen_y < win_h-1; screen_y++)
	{
		/* Find ray length of Intersection with floor */
		floorIntLen /= cos(DEG2RAD((angle-player.view_angle)));

		/* Find coords of intersection with floor */
		floorIntY = player.y - cos(M_PI_2-DEG2RAD(angle)) * floorIntLen;
		floorIntX = player.x + sin(M_PI_2-DEG2RAD(angle)) * floorIntLen;

		/* Find texture coords */
		textX = abs(floorIntX % map.block);
		textY = abs(floorIntY % map.block);

		color = getpixel(floor_text, textX, textY);

		pixels = SDL_GetWindowSurface(sdl_win);
		u32 pix_color = getpixel(pixels, screen_x, screen_y);

		if(pix_color != (u32)PACK_COLOR(0, 0, 0))
		{
			r = (color >> 16) & 255;
			g = (color >> 8) & 255;
			b = color & 255;

			SDL_SetRenderDrawColor(sdl_rend, r, g, b, 255);
			SDL_RenderDrawPoint(sdl_rend, screen_x, screen_y);
			SDL_RenderDrawPoint(sdl_rend, screen_x, win_h-screen_y);
		}

		alpha -= epsilon;
		floorIntLen = tanf(DEG2RAD(alpha)) * ph;
	}
}

