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
	load_ppm("textures/wood_1.ppm", floor_ppm);
	load_ppm("textures/gray_brick_2.ppm", ceil_ppm);

	fisheye_on = 0;
	lowpoly_on = 0;
	wallTextures_on = 1;
	planesTextures_on = 1;

	int run;
	int fullscreen;

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
	fullscreen = atoi(argv[3]);

	if(fullscreen)
		fullscreen = SDL_WINDOW_FULLSCREEN;
	else fullscreen = 0;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	SDL_ShowCursor(SDL_DISABLE);

	sdl_win = SDL_CreateWindow
	(
		"KCast",
		SDL_WINDOWPOS_CENTERED_MASK,
		SDL_WINDOWPOS_CENTERED_MASK,
		win_w,
		win_h,
		fullscreen
	);
	sdl_rend = SDL_CreateRenderer(sdl_win, -1, SDL_RENDERER_SOFTWARE);
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
	SDL_Texture* missing_texture =
		load_texture("textures/missing_texture.png", &IMG_Load);

	floor_text =
		IMG_Load("textures/wolf3d/wood_1.png");
	floor_text =
		SDL_ConvertSurfaceFormat(floor_text, SDL_PIXELFORMAT_ARGB8888, 0);

	ceil_text =
		IMG_Load("textures/wolf3d/gray_brick_2.png");
	ceil_text =
		SDL_ConvertSurfaceFormat(ceil_text, SDL_PIXELFORMAT_ARGB8888, 0);
	
	SDL_Texture* blue_brick =
		load_texture("textures/wolf3d/blue_brick_1.png", &IMG_Load);

	SDL_Texture* gray_brick_1 =
		load_texture("textures/wolf3d/gray_brick_1.png", &IMG_Load);

	SDL_Texture* gray_brick_2 =
		load_texture("textures/wolf3d/gray_brick_2.png", &IMG_Load);

	SDL_Texture* gray_brick_3 =
		load_texture("textures/wolf3d/gray_brick_3.png", &IMG_Load);

	SDL_Texture* multicolor_brick =
		load_texture("textures/wolf3d/multicolor_brick.png", &IMG_Load);

	SDL_Texture* red_brick =
		load_texture("textures/wolf3d/red_brick_1.png", &IMG_Load);

	SDL_Texture* eagle =
		load_texture("textures/wolf3d/eagle.png", &IMG_Load);

	maptext_init();
	maptext_insert('0', missing_texture);
	maptext_insert('1', red_brick);
	maptext_insert('2', eagle);
	maptext_insert('3', gray_brick_1);
	maptext_insert('4', gray_brick_2);
	maptext_insert('5', gray_brick_3);
	maptext_insert('6', multicolor_brick);
	maptext_insert('7', blue_brick);

	load_map_file("map.txt");

	player.x = 1.5 * map.block;
	player.y = 1.5 * map.block;
	player.view_angle = 0.00;

	player.fov = 60.00;
	player.plane_dist = (win_w/2) / tanf(DEG2RAD(player.fov/2));
	player.beta = RAD2DEG(atanf((float)player.plane_dist/(win_w/2)));
	player.ph = (float)map.block/2; /* player's height */
	player.epsilon = (90.0-player.beta)/(win_h/2);

	int a = player.view_angle;
	run = 1;

	while(run)
	{
		realtime = SDL_GetTicks();
		fps = frames_count / (realtime / 1000.0);

		/*
		printf("FPS: %.2f\n", fps);
		*/

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
					case SDLK_ESCAPE:
						run = 0;
						break;

					/* Turn textures */

					case SDLK_t:
						if(wallTextures_on) wallTextures_on = 0;
						else wallTextures_on = 1;
						break;

					case SDLK_y:
						if(planesTextures_on) planesTextures_on = 0;
						else planesTextures_on = 1;
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
