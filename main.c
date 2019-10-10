#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "kcast.h"

int main(int argc, char **argv)
{
	int kc_run;
	kc_screen_t kc_screen;

	SDL_Window *sdl_win;
	SDL_Renderer *sdl_rend;
	SDL_Texture *screen_sdl_text;
	SDL_Event sdl_event;

	if(argc < 4)
	{
		printf(
		"Usage: ./kcast [WIDTH] [HEIGHT] [FULLSCREEN]\n"\
		"FULLSCREEN: 0 - false, 1 - true\n"
		);
		return 0;
	}

	kc_screen.w = atoi(argv[1]);
	kc_screen.h = atoi(argv[2]);
	kc_screen.pixels = malloc(kc_screen.w * kc_screen.h * sizeof(uint32_t));

	SDL_Init(SDL_INIT_VIDEO);
	sdl_win = SDL_CreateWindow
	(
		"KCast",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		kc_screen.w,
		kc_screen.h,
		SDL_WINDOW_SHOWN
	);
	sdl_rend = SDL_CreateRenderer(sdl_win, -1, SDL_RENDERER_ACCELERATED);
	screen_sdl_text = SDL_CreateTexture
	(
		sdl_rend,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		kc_screen.w,
		kc_screen.h
	);

	kc_run = 1;

	while(kc_run)
	{
		while(SDL_PollEvent(&sdl_event) != 0)
		{
			if(sdl_event.type == SDL_QUIT)
			{
				kc_run = 0;
			}
		}

/*
========================================================================
						ENGINE UPDATES
========================================================================
*/

		kc_3d_refresh(&kc_screen);

/*
========================================================================
						SDL UPDATES
========================================================================
*/
		SDL_UpdateTexture
		(
			screen_sdl_text,
			NULL,
			kc_screen.pixels,
			kc_screen.w * sizeof(uint32_t)
		);
		SDL_RenderClear(sdl_rend);
		SDL_RenderCopy(sdl_rend, screen_sdl_text, NULL, NULL);
		SDL_RenderPresent(sdl_rend);
	}

	SDL_DestroyTexture(screen_sdl_text);
	SDL_DestroyRenderer(sdl_rend);
	SDL_DestroyWindow(sdl_win);
	free(kc_screen.pixels);
	return 0;
}
