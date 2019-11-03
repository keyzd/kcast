#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "kcast.h"

SDL_Texture* loadTexture(char* path, SDL_Renderer* sdl_rend)
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

void maptext_init(maptext_t *mt)
{
	mt->count = 0;
	mt->limit = 8;
	mt->data = malloc(mt->limit * sizeof(pair_t));
}

void maptext_insert(maptext_t *mt, char wall, SDL_Texture *sdl_text)
{
	if(mt->count == mt->limit)
	{
		pair_t *new_data = malloc(mt->limit * 2 * sizeof(pair_t));

		for(int i = 0; i < mt->count; i++)
		{
			new_data[i] = mt->data[i];
		}

		mt->limit *= 2;
		mt->data = malloc(mt->limit * sizeof(pair_t));

		for(int i = 0; i < mt->count; i++)
		{
			mt->data[i] = new_data[i];
		}
	}

	pair_t new_pair;
	new_pair.wall = wall;
	new_pair.sdl_text = sdl_text;

	mt->data[mt->count] = new_pair;
	mt->count++;
}

SDL_Texture* maptext_find(maptext_t *mt, char wall)
{
	for(int i = 0; i < mt->count; i++)
	{
		if(mt->data[i].wall == wall)
			return mt->data[i].sdl_text;
	}

	return NULL;
}
