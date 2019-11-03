#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

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
	int w, h;
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
							GLOBALS
========================================================================
*/

int win_w, win_h;
SDL_Renderer *sdl_rend;
maptext_t maptext;
int debug;
map_t map;
player_t player;


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
						MISC.C DEFINITIONS
========================================================================
*/

SDL_Texture* load_texture(char *path);

void maptext_init();
void maptext_insert(char wall, SDL_Texture *sdl_text);
SDL_Texture* maptext_find(char wall);


/*
========================================================================
						RENDERER.C DEFINITIONS
========================================================================
*/

/* Main rendering function */
void ThreeD_refresh();

/* Draw floor and ceiling */
void clear_screen();

void wall_refresh();

/* Main raycasting function */
int raycast(
		int *wall_i,
		int *side,
		uint32_t *column_col,
		float angle
		);

float verticalgrid_intersection(
		int *wall_i,
		int *side,
		uint32_t *column_col,
		float angle
		);

float horizontalgrid_intersection(
		int *wall_i,
		int *side,
		uint32_t *column_col,
		float angle
		);

int get_column_len(float angle, int ray_len, uint32_t *column_col);

void draw_column(
		char wall,
		int side,
		int column_len,
		uint32_t column_col,
		int column_x
		);


/*
========================================================================
						DEBUG.C DEFINITIONS
========================================================================
*/
void map_view_update();

/* TODO */
/*
void map_view_walls();
*/

void map_view_player();

void intersect_draw(int x, int y, uint32_t color);
