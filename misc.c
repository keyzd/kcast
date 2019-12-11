#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

#include "kcast.h"

SDL_Surface* LoadBMP(const char* file)
{
	return SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"), 1);
}

SDL_Texture* load_texture(char* path, SDL_Surface* (*loadFunc)(const char*))
{
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = (*loadFunc)(path);
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

u32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    u8 *p = (u8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(u16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(u32 *)p;
        break;

    default:
        return 0; /* shouldn't happen, but avoids warnings */
    }
}
