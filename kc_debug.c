#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "kcast.h"

void kc_map_view_update(
		kc_screen_t *kc_screen,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	int x;

	for(x = 0; x < kc_screen->w * kc_screen->h; x++)
	{
		kc_screen->pixels[x] = KC_PACK_COLOR(16, 16, 16);
	}

	kc_map_view_walls(kc_screen, kc_map, kc_player);
	kc_map_view_player(kc_screen, kc_map, kc_player);

}

void kc_map_view_walls(
		kc_screen_t *kc_screen,
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

void kc_map_view_player(
		kc_screen_t *kc_screen,
		kc_map_t *kc_map,
		kc_player_t *kc_player)
{
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			kc_screen->pixels[
					(kc_player->unit_x+j)+(kc_player->unit_y+i)*kc_screen->w
					] = KC_PACK_COLOR(255, 255, 255);

		}
	}
}

void kc_intersect_draw(kc_screen_t *kc_screen, int x, int y, uint32_t color)
{
	if(x >= kc_screen->w || y >= kc_screen->h || x < 0 || y < 0)
		return;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			kc_screen->pixels[(x+j)+(y+i)*kc_screen->w] = color;
		}
	}
}
