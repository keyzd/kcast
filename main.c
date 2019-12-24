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
	fisheye_on = 0;
	lowpoly_on = 0;
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
	/*
	SDL_Texture *default_wall = load_texture("../debug_text.png", &IMG_Load);
	SDL_Texture *gray_brick1 = load_texture("../wolfpack/WALL0.bmp", &LoadBMP);
	SDL_Texture *gray_brick2 = load_texture("../wolfpack/WALL68.bmp", &LoadBMP);
	SDL_Texture *blue_brick = load_texture("../wolfpack/WALL16.bmp", &LoadBMP);
	SDL_Texture *red_brick = load_texture("../wolfpack/WALL32.bmp", &LoadBMP);
	SDL_Texture *red_brick_eagle =
		load_texture("../wolfpack/WALL38.bmp", &LoadBMP);
	SDL_Texture *multicolor_brick =
		load_texture("../wolfpack/WALL74.bmp", &LoadBMP);
	*/

	SDL_Texture *wall_light = load_texture("/home/keyzd/Downloads/textures/wall_light.png", &IMG_Load);
	SDL_Texture *comp1 = load_texture("/home/keyzd/Downloads/textures/comp1.png", &IMG_Load);
	SDL_Texture *comp2 = load_texture("/home/keyzd/Downloads/textures/comp2.png", &IMG_Load);
	SDL_Texture *danger_wall = load_texture("/home/keyzd/Downloads/textures/danger_wall.png", &IMG_Load);
	SDL_Texture *box = load_texture("/home/keyzd/Downloads/textures/q2_strogg_box.png", &IMG_Load);
	SDL_Texture *hellsign = load_texture("/home/keyzd/Downloads/textures/666.png", &IMG_Load);
	SDL_Texture *steel_wall = load_texture("/home/keyzd/Downloads/textures/steel_wall.png", &IMG_Load);
	SDL_Texture *steel_wall2 = load_texture("/home/keyzd/Downloads/textures/steel_wall2.png", &IMG_Load);
	SDL_Texture *normal_wall = load_texture("/home/keyzd/Downloads/textures/normal_wall.png", &IMG_Load);
	
	//floor_text = IMG_Load("../wolfpack/WALL52.bmp");
	//floor_text = IMG_Load("../square2.png");
	floor_text = IMG_Load("/home/keyzd/Downloads/textures/floor.png");
	floor_text = SDL_ConvertSurfaceFormat(floor_text, SDL_PIXELFORMAT_ARGB8888, 0);

	//ceil_text = IMG_Load("../wolfpack/WALL22.bmp");
	//ceil_text = IMG_Load("../square.png");
	ceil_text = IMG_Load("/home/keyzd/Downloads/textures/ceiling_lights2.png");
	ceil_text = SDL_ConvertSurfaceFormat(ceil_text, SDL_PIXELFORMAT_ARGB8888, 0);
	


	maptext_init();
	maptext_insert('0', wall_light);
	maptext_insert('1', comp1);
	maptext_insert('2', comp2);
	maptext_insert('3', danger_wall);
	maptext_insert('4', box);
	maptext_insert('5', normal_wall);
	maptext_insert('6', hellsign);
	maptext_insert('7', steel_wall);

	map.block = 64;
	map.grid_w = 18;
	map.grid_h = 18;

	map.w = map.block * map.grid_w;
	map.h = map.block * map.grid_h;
	map.grid = malloc((map.grid_w * map.grid_h + 1) * sizeof(char));
	grid = 
		"111111111111111111"\
		"5                1"\
		"5                1"\
		"5   4   55555    1"\
		"6      6         1"\
		"5      0         1"\
		"5      6         1"\
		"5    34444       1"\
		"1    3   233333  1"\
		"1    3   2       1"\
		"1    3   2 11111 1"\
		"1        2       1"\
		"1        2   7   1"\
		"1  7777652   7   1"\
		"1            7   1"\
		"1                1"\
		"1                1"\
		"111111111111111111";

	strcpy(map.grid, grid);

	player.x = 2 * map.block;
	player.y = 2 * map.block;
	player.view_angle = 270.00;

	player.fov = 60.00;
	player.plane_dist = (win_w/2) / tanf(DEG2RAD(player.fov/2));
	player.beta = RAD2DEG(atanf((float)player.plane_dist/(win_w/2)));
	player.ph = (float)map.block/2; /* player's height */

	int a = player.view_angle;

	isRun = 1;
	while(isRun)
	{
		realtime = SDL_GetTicks();
		fps = frames_count / (realtime / 1000.0);

		//printf("FPS: %.2f\n", fps);

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
						printf("FOV: %f\n", player.fov);
						break;

					case SDLK_f:
						if(fisheye_on) fisheye_on = 0;
						else fisheye_on = 1;
						break;

					case SDLK_m:
						player.fov++;
						break;

					case SDLK_n:
						player.fov--;
						break;

					case SDLK_l:
						if(lowpoly_on) lowpoly_on = 0;
						else lowpoly_on = 1;
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
