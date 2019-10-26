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
		ray_len =
			kc_raycast(kc_screen, kc_map, kc_player, &column_col, angle);
		column_len = kc_get_column_len(kc_screen, kc_player, angle, ray_len, &column_col);
		kc_draw_column(kc_screen, column_len, column_col, column_x);
		angle -= angle_step;
	}
}

int kc_raycast(
		kc_screen_t *kc_screen,
		kc_map_t* kc_map,
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle)
{
	uint32_t color_vert, color_horiz; 
	float len_vert =
		kc_verticalgrid_intersection(kc_screen, kc_map, kc_player,
									 &color_vert, angle);
	float len_horiz =
		kc_horizontalgrid_intersection(kc_screen, kc_map, kc_player,
									   &color_horiz, angle);

	if(len_vert < len_horiz)
	{
		*column_col = color_vert;
		return (int)len_vert;
	}
	else
	{
		*column_col = color_horiz;
		return (int)len_horiz;
	}
}

float kc_horizontalgrid_intersection(
		kc_screen_t *kc_screen,
		kc_map_t* kc_map, 
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

	if(((int)angle == 180) || ((int)angle == 0) || ((int)angle == 360))
		return 10000000.0;

	if(angle > 0 && angle < 180) /* Ray faced up */
		Ay = (kc_player->unit_y / kc_map->block) * kc_map->block - 1;
	else
		Ay =
			(kc_player->unit_y / kc_map->block) * kc_map->block + kc_map->block;
	
	Ax = kc_player->unit_x+(kc_player->unit_y-Ay)*tanf(M_PI_2-DEG2RAD(angle));

	deltaY = kc_map->block;
	if(angle > 0 && angle < 180) /* Ray faced up */
		deltaY = 0 - deltaY;
	deltaX = kc_map->block * tanf(M_PI_2 - DEG2RAD(angle));

	if(angle > 270)
		deltaX = 0 - deltaX;
	if(angle > 180 && angle < 270)
		deltaX = 0 - deltaX;
		

	hit = 0;
	currentX = Ax;
	currentY = Ay;
	
	while(!hit)
	{
		currentGridX = currentX / kc_map->block;
		currentGridY = currentY / kc_map->block;

		if(currentX > kc_map->grid_w*kc_map->block || currentX < 0 ||
			currentY > kc_map->grid_h*kc_map->block || currentY < 0)
			break;

		kc_intersect_draw(kc_screen, currentX, currentY,
							KC_PACK_COLOR(0, 255, 0));
		if(kc_map->grid[currentGridX + currentGridY * kc_map->grid_w] != ' ')
		{
			hit = 1;
		}
		else
		{
			currentX += deltaX;
			currentY += deltaY;
		}
	}

	switch(kc_map->grid[currentGridX + currentGridY * kc_map->grid_w])
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

	rayX = kc_player->unit_x - currentX;
	rayY = kc_player->unit_y - currentY;
	rayLen = sqrt(rayX*rayX + rayY*rayY);

	return rayLen;
}

float kc_verticalgrid_intersection(
		kc_screen_t *kc_screen,
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
	int flag = 0;

	if(((int)angle == 90) || ((int)angle == 89) || ((int)angle == 91) ||
		((int)angle == 270) || ((int)angle == 269) || ((int)angle == 271))
		return 10000000.0;

	if((int)angle < 90 || (int)angle > 270) /* Ray faced right */
		Bx = (kc_player->unit_x/kc_map->block)*kc_map->block+kc_map->block;
	else
		Bx = (kc_player->unit_x/kc_map->block)*kc_map->block-1;
	
	By = kc_player->unit_y+(kc_player->unit_x-Bx)*tanf(DEG2RAD(angle));

	deltaX = kc_map->block;
	if((int)angle > 90 && (int)angle < 270)
		deltaX = 0 - kc_map->block;
	
	deltaY = fabsf(kc_map->block * tanf(DEG2RAD(angle)));
	if((int)angle > 0 && (int)angle < 180) /* Ray faced up */
		deltaY = 0 - deltaY;

	hit = 0;
	currentX = Bx;
	currentY = By;
	
	while(!hit)
	{
		currentGridX = currentX / kc_map->block;
		currentGridY = currentY / kc_map->block;

		if(currentX > kc_map->grid_w*kc_map->block || currentX < 0 ||
			currentY > kc_map->grid_h*kc_map->block || currentY < 0)
			break;

		kc_intersect_draw(kc_screen, currentX, currentY,
							KC_PACK_COLOR(255, 0, 0));
		if(kc_map->grid[currentGridX + currentGridY * kc_map->grid_w] != ' ')
		{
			hit = 1;
		}
		else
		{
			currentX += deltaX;
			currentY += deltaY;
		}

	}

	switch(kc_map->grid[currentGridX + currentGridY * kc_map->grid_w])
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
			*column_col = KC_PACK_COLOR(200, 255, 0);
			break;
	}

	rayX = kc_player->unit_x - currentX;
	rayY = kc_player->unit_y - currentY;
	rayLen = sqrt(rayX*rayX + rayY*rayY);

	return rayLen;
}

int kc_get_column_len(
		kc_screen_t *kc_screen, 
		kc_player_t *kc_player,
		float angle,
		int ray_len,
		uint32_t *column_col)
{
	float len;
	//len = kc_screen->h / (ray_len * cos(DEG2RAD(angle-kc_player->view_angle)));
	len = 24 * kc_screen->h / ray_len;
	return (int)len;
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
