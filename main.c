#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

#include "kcast.h"

#define FPS_LOCK 60.0

int main(int argc, char *argv[])
{
	fisheye = 0;
	turnTextures = 0;
	int isRun;
	char *grid;
	int isFullscreen;

	u32 realtime, frames_count;
	float fps;
	frames_count = 0;

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
	isFullscreen = atoi(argv[3]);

	if(isFullscreen)
		isFullscreen = SDL_WINDOW_FULLSCREEN_DESKTOP;
	else isFullscreen = 0;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	sdl_win = SDL_CreateWindow
	(
		"KCast",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		win_w,
		win_h,
		SDL_WINDOW_SHOWN | isFullscreen
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

/*
========================================================================
							PREPARING TEXTURES
========================================================================
*/
	SDL_Texture *default_wall = load_texture("../debug_text.png", &IMG_Load);
	SDL_Texture *gray_brick1 = load_texture("../wolfpack/WALL0.bmp", &LoadBMP);
	SDL_Texture *gray_brick2 = load_texture("../wolfpack/WALL68.bmp", &LoadBMP);
	SDL_Texture *blue_brick = load_texture("../wolfpack/WALL16.bmp", &LoadBMP);
	SDL_Texture *red_brick = load_texture("../wolfpack/WALL32.bmp", &LoadBMP);
	SDL_Texture *red_brick_eagle =
		load_texture("../wolfpack/WALL38.bmp", &LoadBMP);
	SDL_Texture *multicolor_brick =
		load_texture("../wolfpack/WALL74.bmp", &LoadBMP);
	
	floor_text = IMG_Load("../wolfpack/WALL52.bmp");
	//floor_text = IMG_Load("../square.png");
	floor_text = SDL_ConvertSurfaceFormat(floor_text, SDL_PIXELFORMAT_ARGB8888, 0);

	ceil_text = IMG_Load("../wolfpack/WALL22.bmp");
	//ceil_text = IMG_Load("../square2.png");
	ceil_text = SDL_ConvertSurfaceFormat(ceil_text, SDL_PIXELFORMAT_ARGB8888, 0);
	


	maptext_init();
	maptext_insert('0', default_wall);
	maptext_insert('1', gray_brick1);
	maptext_insert('2', gray_brick2);
	maptext_insert('3', blue_brick);
	maptext_insert('4', red_brick);
	maptext_insert('5', red_brick_eagle);
	maptext_insert('6', multicolor_brick);

	map.block = 64;

	/*
	map.grid_w = 32;
	map.grid_h = 26;
	*/

	map.grid_w = 18;
	map.grid_h = 18;

	map.w = map.block * map.grid_w;
	map.h = map.block * map.grid_h;
	map.grid = malloc(map.grid_w * map.grid_h * sizeof(char));
	grid = 
		"111111111111111111"\
		"1                1"\
		"1                1"\
		"1       55555    1"\
		"1      6         1"\
		"1      6         1"\
		"1      6         1"\
		"1    44444       1"\
		"1    3   233333  1"\
		"1    3   2       1"\
		"1    3   2 11111 1"\
		"1        2       1"\
		"1        2   4   1"\
		"1  6666662   4   1"\
		"1            4   1"\
		"1                1"\
		"1                1"\
		"111111111111111111";
		/*
		"1111111111111111"\
		"1    1   1     1"\
		"1              1"\
		"1    1   1     1"\
		"111111   1111111"\
		"1              1"\
		"1              1"\
		"1              1"\
		"1111111111111111";
		*/
	/*
	"11111111111111111111111111111111"\
	"2                              2"\
	"2                              2"\
	"2  s                           2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"2                              2"\
	"33333333333333333333333333333332";
	*/

	strcpy(map.grid, grid);

	player.x = 2 * map.block;
	player.y = 2 * map.block;
	player.view_angle = 270.00;

	player.fov = 60.00;
	player.plane_dist = (win_w/2) / tanf(DEG2RAD(player.fov/2));

	int a = player.view_angle;

	isRun = 1;
	while(isRun)
	{
		realtime = SDL_GetTicks();
		fps = frames_count / (realtime / 1000.0);

		//printf("\rFPS: %.2f", fps);

		while(SDL_PollEvent(&sdl_event) != 0)
		{
			if(sdl_event.type == SDL_QUIT)
			{
				isRun = 0;
			}
			else if(sdl_event.type == SDL_KEYDOWN)
			{
				switch(sdl_event.key.keysym.sym)
				{
					/* Turn textures */
					case SDLK_t:
						if(turnTextures) turnTextures = 0;
						else turnTextures = 1;
						break;
					
					case SDLK_p:
						printf("\nView angle: %f\n", player.view_angle);
						break;

					case SDLK_f:
						if(fisheye) fisheye = 0;
						else fisheye = 1;
						break;


					/* PLAYER CONTROLS */

					case SDLK_LEFT:
						a+=3;
						player.view_angle = a % 360;
						//printf("\nView angle: %f\n", player.view_angle);
						break;

					case SDLK_RIGHT:
						a-=3;
						if(a < 0)
							a = 360 - abs(a);
						player.view_angle = a % 360;
						//printf("\nView angle: %f\n", player.view_angle);
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

		threeD_refresh();

		SDL_RenderPresent(sdl_rend);

		/* Set FPS Lock */
		SDL_Delay(1.0/FPS_LOCK*1000);
		frames_count++;
	}

	SDL_DestroyTexture(screen_sdl_text);
	SDL_DestroyRenderer(sdl_rend);
	SDL_DestroyWindow(sdl_win);
	free(map.grid);
	return 0;
}
