#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "kcast.h"

void kc_map_view_update(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	SDL_Rect screen_rect;

	screen_rect.x = 0;
	screen_rect.y = 0;
	screen_rect.w = win_w;
	screen_rect.h = win_h;

	SDL_SetRenderDrawColor(sdl_rend, 16, 16, 16, 255);
	SDL_RenderFillRect(sdl_rend, &screen_rect);
}

/*
void kc_map_view_walls(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	for(uint32_t i = 0; i < kc_map->grid_w * kc_map->grid_h; i++)
	{
		if(kc_map->grid[i] != ' ')
		{
			uint32_t level, first_reduced, first_origin, address;

			level = i / kc_map->grid_w;
			first_reduced = level * kc_map->grid_w;
			first_origin = first_reduced * kc_map->block * kc_map->block;

			address = first_origin + ( (i - first_reduced) * kc_map->block );

			for(uint32_t j = 0; j < kc_map->block; j++)
			{
				for(uint32_t k = address; k < address+(kc_map->block); k++)
				{
					switch(kc_map->grid[i])
					{
						case '1':
							kc_screen->pixels[k] =
								KC_PACK_COLOR(192, 0, 0);
							break;

						case '2':
							kc_screen->pixels[k] =
								KC_PACK_COLOR(0, 192, 0);
							break;

						case '3':
							kc_screen->pixels[k] =
								KC_PACK_COLOR(0, 0, 192);
							break;

						case '4':
							kc_screen->pixels[k] =
								KC_PACK_COLOR(192, 192, 192);
							break;
					}
				}

				address += kc_screen->w;
			}
		}
	}
}
*/

void kc_map_view_player(
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	SDL_Rect player_rect;

	player_rect.x = kc_player->unit_x;
	player_rect.y = kc_player->unit_y;
	player_rect.h = 2;
	player_rect.w = 2;

	SDL_SetRenderDrawColor(sdl_rend, 255, 255, 255, 255);
	SDL_RenderFillRect(sdl_rend, &player_rect);
}

void kc_intersect_draw(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		int x, int y,
		uint32_t color)
{
	SDL_Rect point;
	uint8_t r, g, b;

	point.x = x;
	point.y = y;
	point.h = 2;
	point.w = 2;

	r = (color >> 16) & 255;
	g = (color >> 8) & 255;
	b = color & 255;

	/*
	if(x >= win_w || y >= win_h || x < 0 || y < 0)
		return;
	*/

	SDL_SetRenderDrawColor(sdl_rend, r, g, b, 255);
	SDL_RenderFillRect(sdl_rend, &point);
}
