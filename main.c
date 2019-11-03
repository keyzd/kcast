#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "kcast.h"

int main(int argc, char **argv)
{
	debug = 1;
	int run;
	map_t map;
	int win_w, win_h;
	char *grid;
	int fullscreen;

	uint32_t realtime, frames;
	float fps;
	frames = 0;

	maptext_t maptext;

	int map_view;
	map_view = 0;

	player_t player;

	SDL_Window *sdl_win;
	SDL_Renderer *sdl_rend;
	SDL_Texture *screen_sdl_text;
	SDL_Texture *wall_brick1, *wall_brick2, *wall_test;
	SDL_Event sdl_event;

	if(argc < 4)
	{
		printf(
		"Usage: ./kcast [WIDTH] [HEIGHT] [FULLSCREEN]\n"\
		"FULLSCREEN: 0 - false, 1 - true\n"
		);
		return 0;
	}

	win_w = atoi(argv[1]);
	win_h = atoi(argv[2]);
	fullscreen = atoi(argv[3]);

	if(fullscreen)
		fullscreen = SDL_WINDOW_FULLSCREEN_DESKTOP;
	else fullscreen = 0;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	sdl_win = SDL_CreateWindow
	(
		"KCast",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		win_w,
		win_h,
		SDL_WINDOW_SHOWN | fullscreen
	);
	sdl_rend = SDL_CreateRenderer(sdl_win, -1, SDL_RENDERER_ACCELERATED);
	screen_sdl_text = SDL_CreateTexture
	(
		sdl_rend,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		win_w,
		win_h
	);

	wall_brick1 = loadTexture("../brick.png", sdl_rend);
	wall_brick2 = loadTexture("../brick2.png", sdl_rend);
	wall_test = loadTexture("../wall.png", sdl_rend);

	maptext_init(&maptext);
	maptext_insert(&maptext, '1', wall_brick1);
	maptext_insert(&maptext, '2', wall_brick2);
	maptext_insert(&maptext, '3', wall_brick1);
	maptext_insert(&maptext, '4', wall_test);

	map.block = 32;
	map.grid_w = 16;
	map.grid_h = 16;
	map.grid = malloc(map.grid_w * map.grid_h * sizeof(char));
	grid = 
		"4121111111111111"\
		"4              2"\
		"4     11111    2"\
		"4    2         2"\
		"4    2  33331112"\
		"4    2         2"\
		"4  221333      2"\
		"4  3    24444  2"\
		"4  3    2      2"\
		"4  3    2  31432"\
		"4  3    2      2"\
		"4       2      2"\
		"4 1111111      2"\
		"4              2"\
		"4              2"\
		"3333333333333332";
		/*
		"4111111111111111"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"4              2"\
		"3333333333333332";
		*/
		/*
		"12121212"\
		"2      1"\
		"1      2"\
		"2      1"\
		"1      2"\
		"2      1"\
		"1      2"\
		"21212121";
		*/

	strcpy(map.grid, grid);

	player.x = 2 * map.block;
	player.y = 2 * map.block;
	player.fov = 60.0000;
	player.view_angle = 0.00;

	run = 1;
	while(run)
	{
		realtime = SDL_GetTicks();
		fps = frames / (realtime / 1000.0);

		printf("\rFPS: %.2f", fps);

		while(SDL_PollEvent(&sdl_event) != 0)
		{
			if(sdl_event.type == SDL_QUIT)
			{
				run = 0;
			}
			else if(sdl_event.type == SDL_KEYDOWN)
			{
				switch(sdl_event.key.keysym.sym)
				{
					/* DEBUG OPTIONS */

					/* Textures */
					case SDLK_t:
						if(debug) debug = 0;
						else debug = 1;
						break;

					/* Map view */
					case SDLK_m:
						if(map_view) map_view = 0;
						else map_view = 1;
						break;

					/* PLAYER CONTROLS */

					case SDLK_LEFT:
						player.view_angle += 3;
						if(player.view_angle > 360)
						{
							int tmp = player.view_angle - 357;
							player.view_angle = -3 + tmp;
						}
						if(player.view_angle == 360)
							player.view_angle = 0;
						//printf("View angle: %f\n", player.view_angle);
						break;

					case SDLK_RIGHT:
						player.view_angle -= 3;
						if(player.view_angle < 0)
						{
							player.view_angle = 354 - player.view_angle;
						}
						if(player.view_angle == 360)
							player.view_angle = 0;
						//printf("View angle: %f\n", player.view_angle);
						break;

					case SDLK_w:
						player.x +=
							8*cos(DEG2RAD(player.view_angle));
						player.y -=
							8*sin(DEG2RAD(player.view_angle));
						break;

					case SDLK_s:
						player.x -=
							8*sin(M_PI_2 + DEG2RAD(player.view_angle));
						player.y -=
							8*cos(M_PI_2 + DEG2RAD(player.view_angle));
						break;

					case SDLK_a:
						player.x -=
							8*sin(DEG2RAD(player.view_angle));
						player.y -=
							8*cos(DEG2RAD(player.view_angle));
						break;

					case SDLK_d:
						player.x +=
							8*sin(DEG2RAD(player.view_angle));
						player.y +=
							8*cos(DEG2RAD(player.view_angle));
						break;
				}
			}
		}
/*
========================================================================
						SDL AND ENGINE UPDATES
========================================================================
*/

		SDL_RenderClear(sdl_rend);
		SDL_RenderCopy(sdl_rend, screen_sdl_text, NULL, NULL);
		if(!map_view)
			ThreeD_refresh(&maptext, win_w, win_h, sdl_rend, &map, &player);
		else map_view_update(win_w, win_h, sdl_rend, &map, &player);
		SDL_RenderPresent(sdl_rend);


		/* FPS lock */
		SDL_Delay((1.0/60.0*1000));
		frames++;
	}

	SDL_DestroyTexture(screen_sdl_text);
	SDL_DestroyRenderer(sdl_rend);
	SDL_DestroyWindow(sdl_win);
	free(map.grid);
	return 0;
}
