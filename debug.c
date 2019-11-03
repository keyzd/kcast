#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "kcast.h"

void map_view_update()
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
void map_view_walls()
{
	for(uint32_t i = 0; i < map.grid_w * map.grid_h; i++)
	{
		if(map.grid[i] != ' ')
		{
			uint32_t level, first_reduced, first_origin, address;

			level = i / map.grid_w;
			first_reduced = level * map.grid_w;
			first_origin = first_reduced * map.block * map.block;

			address = first_origin + ( (i - first_reduced) * map.block );

			for(uint32_t j = 0; j < map.block; j++)
			{
				for(uint32_t k = address; k < address+(map.block); k++)
				{
					switch(map.grid[i])
					{
						case '1':
							screen->pixels[k] =
								PACK_COLOR(192, 0, 0);
							break;

						case '2':
							screen->pixels[k] =
								PACK_COLOR(0, 192, 0);
							break;

						case '3':
							screen->pixels[k] =
								PACK_COLOR(0, 0, 192);
							break;

						case '4':
							screen->pixels[k] =
								PACK_COLOR(192, 192, 192);
							break;
					}
				}

				address += screen->w;
			}
		}
	}
}
*/

void map_view_player()
{
	SDL_Rect player_rect;

	player_rect.x = player.x;
	player_rect.y = player.y;
	player_rect.h = 2;
	player_rect.w = 2;

	SDL_SetRenderDrawColor(sdl_rend, 255, 255, 255, 255);
	SDL_RenderFillRect(sdl_rend, &player_rect);
}

void intersect_draw(int x, int y, uint32_t color)
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
