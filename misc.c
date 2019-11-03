#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "kcast.h"

SDL_Texture* load_texture(char* path)
{
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path);
    if( loadedSurface == NULL )
    {
        printf("Unable to load image %s! SDL_image Error: %s\n",
				path, IMG_GetError());
    }
    else
    {
        newTexture = SDL_CreateTextureFromSurface(sdl_rend, loadedSurface);
        if( newTexture == NULL )
        {
            printf("Unable to create texture from %s! SDL Error: %s\n",
					path, SDL_GetError() );
        }

        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

void maptext_init()
{
	maptext.count = 0;
	maptext.limit = 8;
	maptext.data = malloc(maptext.limit * sizeof(pair_t));
}

void maptext_insert(char wall, SDL_Texture *sdl_text)
{
	if(maptext.count == maptext.limit)
	{
		pair_t *new_data = malloc(maptext.limit * 2 * sizeof(pair_t));

		for(int i = 0; i < maptext.count; i++)
		{
			new_data[i] = maptext.data[i];
		}

		maptext.limit *= 2;
		maptext.data = malloc(maptext.limit * sizeof(pair_t));

		for(int i = 0; i < maptext.count; i++)
		{
			maptext.data[i] = new_data[i];
		}
	}

	pair_t new_pair;
	new_pair.wall = wall;
	new_pair.sdl_text = sdl_text;

	maptext.data[maptext.count] = new_pair;
	maptext.count++;
}

SDL_Texture* maptext_find(char wall)
{
	for(int i = 0; i < maptext.count; i++)
	{
		if(maptext.data[i].wall == wall)
			return maptext.data[i].sdl_text;
	}

	return NULL;
}
