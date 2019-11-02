#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "kcast.h"

int main(int argc, char **argv)
{
	int kc_run;
	kc_map_t kc_map;
	int win_w, win_h;
	char *grid;
	int fullscreen;

	kc_maptext_t maptext;

	int map_view;
	map_view = 0;

	kc_player_t kc_player;

	SDL_Window *sdl_win;
	SDL_Renderer *sdl_rend;
	SDL_Texture *screen_sdl_text;
	SDL_Texture *wall_brick1, *wall_brick2;
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

	kc_maptext_init(&maptext);
	kc_maptext_insert(&maptext, '1', wall_brick1);
	kc_maptext_insert(&maptext, '2', wall_brick2);
	kc_maptext_insert(&maptext, '3', wall_brick1);
	kc_maptext_insert(&maptext, '4', wall_brick2);

	kc_map.block = 64;
	kc_map.grid_w = 16;
	kc_map.grid_h = 16;
	kc_map.grid = malloc(kc_map.grid_w * kc_map.grid_h * sizeof(char));
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

	strcpy(kc_map.grid, grid);

	kc_player.unit_x = 96;
	kc_player.unit_y = 224;
	kc_player.fov = 60.00;
	kc_player.view_angle = 90.00;


	kc_run = 1;
	while(kc_run)
	{
		while(SDL_PollEvent(&sdl_event) != 0)
		{
			if(sdl_event.type == SDL_QUIT)
			{
				kc_run = 0;
			}
			else if(sdl_event.type == SDL_KEYDOWN)
			{
				switch(sdl_event.key.keysym.sym)
				{
					case SDLK_SPACE:
						if(map_view) map_view = 0;
						else map_view = 1;
						break;

					case SDLK_LEFT:
						kc_player.view_angle += 3;
						if(kc_player.view_angle > 360)
						{
							int tmp = kc_player.view_angle - 357;
							kc_player.view_angle = -3 + tmp;
						}
						if(kc_player.view_angle == 360)
							kc_player.view_angle = 0;
						printf("View angle: %f\n", kc_player.view_angle);
						break;

					case SDLK_RIGHT:
						kc_player.view_angle -= 3;
						if(kc_player.view_angle < 0)
						{
							kc_player.view_angle = 354 - kc_player.view_angle;
						}
						if(kc_player.view_angle == 360)
							kc_player.view_angle = 0;
						printf("View angle: %f\n", kc_player.view_angle);
						break;

					case SDLK_w:
						kc_player.unit_x +=
							2*cos(DEG2RAD(kc_player.view_angle));
						kc_player.unit_y -=
							2*sin(DEG2RAD(kc_player.view_angle));
						break;

					case SDLK_s:
						kc_player.unit_x -=
							2*sin(M_PI_2 + DEG2RAD(kc_player.view_angle));
						kc_player.unit_y -=
							2*cos(M_PI_2 + DEG2RAD(kc_player.view_angle));
						break;

					case SDLK_a:
						kc_player.unit_x -=
							2*sin(DEG2RAD(kc_player.view_angle));
						kc_player.unit_y -=
							2*cos(DEG2RAD(kc_player.view_angle));
						break;

					case SDLK_d:
						kc_player.unit_x +=
							2*sin(DEG2RAD(kc_player.view_angle));
						kc_player.unit_y +=
							2*cos(DEG2RAD(kc_player.view_angle));
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
			kc_3d_refresh(&maptext, win_w, win_h, sdl_rend, &kc_map, &kc_player);
		else kc_map_view_update(win_w, win_h, sdl_rend, &kc_map, &kc_player);
		SDL_RenderPresent(sdl_rend);

		SDL_Delay(40);
	}

	SDL_DestroyTexture(screen_sdl_text);
	SDL_DestroyRenderer(sdl_rend);
	SDL_DestroyWindow(sdl_win);
	free(kc_map.grid);
	return 0;
}
