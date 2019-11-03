#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

int debug;

/*
========================================================================
							MACROS
========================================================================
*/

/* ARGB pixel format */
#define PACK_COLOR(R, G, B) ( ( 255<<24) |( R<<16 ) | ( G<<8 ) | B )

#define DEG2RAD(a) ( M_PI/180 * a )
#define RAD2DEG(a) ( 180/M_PI * a )


/*
========================================================================
							TYPES
========================================================================
*/

typedef struct map_s
{
	/* TODO: invent map type and map format */
	char *grid;
	int block;
	int grid_w, grid_h;
}map_t;

typedef struct player_s
{
	int x, y;
	/* Angles are in degrees*/
	float view_angle;
	float fov;
}player_t;

typedef struct pair_s
{
	char wall;
	SDL_Texture *sdl_text;
}pair_t;

typedef struct maptext_s
{
	pair_t *data;
	int count;
	int limit;
}maptext_t;

/*
========================================================================
						MISC.C DEFINITIONS
========================================================================
*/

SDL_Texture* loadTexture(char *path, SDL_Renderer *sdl_rend);

void maptext_init(maptext_t *mt);
void maptext_insert(maptext_t *mt, char wall, SDL_Texture *sdl_text);
SDL_Texture* maptext_find(maptext_t *mt, char wall);


/*
========================================================================
						RENDERER.C DEFINITIONS
========================================================================
*/

/* Main rendering function */
void ThreeD_refresh(
		maptext_t *mt,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		map_t *map,
		player_t *player);

/* Draw floor and ceiling */
void clear_screen(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend, 
		map_t *map);

void wall_refresh(
		maptext_t *mt,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		map_t *map,
		player_t *player);

/* Main raycasting function */
int raycast(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		map_t* map,
		player_t *player,
		uint32_t *column_col,
		float angle);

float verticalgrid_intersection(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		map_t* map,
		player_t *player,
		uint32_t *column_col,
		float angle);

float horizontalgrid_intersection(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		map_t* map,
		player_t *player,
		uint32_t *column_col,
		float angle);

int get_column_len(
		map_t *map,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend, 
		player_t *player_t,
		float angle,
		int ray_len,
		uint32_t *column_col);

void draw_column(
		maptext_t *mt,
		char wall,
		int side,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		int column_len,
		uint32_t column_col,
		int column_x);


/*
========================================================================
						DEBUG.C DEFINITIONS
========================================================================
*/
void map_view_update(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		map_t *map,
		player_t *player);

/*
void map_view_walls(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		map_t *map,
		player_t *player);
*/

void map_view_player(
		SDL_Renderer *sdl_rend,
		map_t *map,
		player_t *player);

void intersect_draw(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		int x, int y,
		uint32_t color);
