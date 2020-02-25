#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "kcast.h"

void threeD_refresh()
{

	if(!planesTextures_on)
		clear_screen();
	draw_columns();
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
	//SDL_SetRenderDrawColor(sdl_rend, 0, 0, 0, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect1);

	SDL_SetRenderDrawColor(sdl_rend, 113, 113, 113, 255);
	//SDL_SetRenderDrawColor(sdl_rend, 0, 0, 0, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect2);
}

void draw_columns()
{
	float angle, angle_step, halffov;
	int column_x;
	int ray_len, column_len;
	u32 column_col;
	int side;
	int wall_i;
	int column_y;

	halffov = player.fov / 2.0;
	angle_step = player.fov / win_w;
	angle = (int)(player.view_angle + halffov) % 360;
	float ang;

	for(column_x = 0; column_x < win_w; column_x++)
	{
		if(lowpoly_on) ang = (int)angle;
		else ang = angle;
		ray_len = raycast(&wall_i, &side, &column_col, ang);
		column_len = get_column_len(ray_len);
		draw_wall_column(wall_i, side, column_len,
						 column_col, column_x, &column_y);

		if(planesTextures_on)
			draw_floor_ceiling_columns(column_x, column_y, ang);

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

	float vertical_intersection_len =
		march_by_vertical_grid(&vertX, &vertY, &wall_i_vert, &side_vert,
									&color_vert, angle);
	float horizontal_intersection_len =
		march_by_horizontal_grid(&horizX, &horizY, &wall_i_horiz,
									&side_horiz, &color_horiz, angle);

	if(vertical_intersection_len < horizontal_intersection_len)
	{
		*column_col = color_vert;
		*side = side_vert;
		*wall_i = wall_i_vert;
		return (int)vertical_intersection_len;
	}
	else
	{
		*column_col = color_horiz;
		*side = side_horiz;
		*wall_i = wall_i_horiz;
		return (int)horizontal_intersection_len;
	}
}

float march_by_horizontal_grid(
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
	if( ( (int)angle >= 176 && (int)angle <= 184 ) ||
		( (int)angle >= 356 || (int)angle <= 4 ))
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

	if(!fisheye_on)
		rayLen *= cos(DEG2RAD(fabsf(angle-player.view_angle)));

	return rayLen;
}

float march_by_vertical_grid(
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
	if( (int)angle == 90 || (int)angle == 270 )
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

	if(!fisheye_on)
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
	if(ray_len <= 0)
		return 1;
	float len = (float)map.block * win_h / ray_len;
	return (int)len;
}

void draw_wall_column(
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
		column_len = win_h;

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
	
	if(!wallTextures_on)
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

void draw_floor_ceiling_columns(int screen_x, int screen_y, float angle)
{
	u8 r, g, b;
	u32 color;

	float beta, epsilon;
	int floorPointDist, floorPointX, floorPointY;
	int textureX, textureY;

	beta = player.beta;
	epsilon = (90.0-beta)/(win_h/2);

	for(int y = win_h-1 ; y > screen_y; y--)
	{
		floorPointDist = tanf(DEG2RAD(beta)) * player.ph;

		if(!fisheye_on)
			floorPointDist /= cos(DEG2RAD((angle-player.view_angle)));

		floorPointY = player.y - cos(M_PI_2-DEG2RAD(angle)) * floorPointDist;
		floorPointX = player.x + sin(M_PI_2-DEG2RAD(angle)) * floorPointDist;

		textureX = abs(floorPointX % map.block);
		textureY = abs(floorPointY % map.block);

		color = getpixel(floor_text, textureX, textureY);
		r = (color >> 16) & 255;
		g = (color >> 8) & 255;
		b = color & 255;
		SDL_SetRenderDrawColor(sdl_rend, r, g, b, 255);
		SDL_RenderDrawPoint(sdl_rend, screen_x, y);

		color = getpixel(ceil_text, textureX, textureY);
		r = (color >> 16) & 255;
		g = (color >> 8) & 255;
		b = color & 255;
		SDL_SetRenderDrawColor(sdl_rend, r, g, b, 255);
		SDL_RenderDrawPoint(sdl_rend, screen_x, win_h-y);

		beta += epsilon;
	}
}

