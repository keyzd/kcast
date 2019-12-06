#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

/*
========================================================================
							TYPES
========================================================================
*/

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

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
	float plane_dist;
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

typedef struct mat_s
{
	int w, h;
	float *data;
}mat_t;


/*
========================================================================
							GLOBALS
========================================================================
*/

int win_w, win_h;
SDL_Renderer *sdl_rend;
maptext_t maptext;
int turnTextures;
map_t map;
player_t player;
SDL_Surface *floor_text;


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

SDL_Surface* LoadBMP(const char* file);
SDL_Texture* load_texture(char* path, SDL_Surface* (*loadFunc)(const char*));

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

/* Draw textured walls */
void wall_refresh();

int raycast(
		int *wall_i,
		int *side,
		u32 *column_col,
		float angle
		);

float verticalgrid_intersection(
		int *rayX, int *rayY,
		int *wall_i,
		int *side,
		u32 *column_col,
		float angle
		);

float horizontalgrid_intersection(
		int *rayX, int *rayY,
		int *wall_i,
		int *side,
		u32 *column_col,
		float angle
		);

int get_column_len(float angle, int ray_len, u32 *column_col);

void draw_column(
		char wall,
		int side,
		int column_len,
		u32 column_col,
		int column_x,
		float angle,
		int *column_y
		);

u32 getpixel(SDL_Surface *surface, int x, int y);
void draw_floor_column(int wall_x, int wall_y, int screen_x,
						int screen_y, float angle);


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

void intersect_draw(int x, int y, u32 color);

