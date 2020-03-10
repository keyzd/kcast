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
	char *grid;
	int block;
	int grid_w, grid_h;
	int w, h;
}map_t;

/* TODO */
typedef struct sprite_s
{
	int x, y;
	int text_id;
}sprite_t;

typedef struct player_s
{
	int x, y;
	/* Angles are in degrees*/
	float view_angle;
	float fov;
	float plane_dist;
	float beta;
	float epsilon;
	float ph;
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

typedef struct pixel_s
{
	uint8_t r, g, b;
}pixel_t;


/*
========================================================================
							GLOBALS
========================================================================
*/

int fisheye_on;
int wallTextures_on;
int planesTextures_on;
int lowpoly_on;

int win_w, win_h;
SDL_Renderer *sdl_rend;
SDL_Window *sdl_win;
SDL_Texture *screen_sdl_text;
maptext_t maptext;
map_t map;
player_t player;
SDL_Surface *floor_text;
SDL_Surface *ceil_text;


pixel_t floor_ppm[64*64];
pixel_t ceil_ppm[64*64];


/*
========================================================================
							MACROS
========================================================================
*/

/* ARGB pixel format */
#define PACK_COLOR(R, G, B) ( ( 255<<24) |( R<<16 ) | ( G<<8 ) | B )

#define DEG2RAD(a) ( M_PI/180 * a )
#define RAD2DEG(a) ( 180/M_PI * a )

#define DEFAULT_BLOCK_SIZE 64

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
u32 getpixel(SDL_Surface *surface, int x, int y);


/*
========================================================================
						RENDERER.C DEFINITIONS
========================================================================
*/

/* Main rendering function */
void threeD_refresh();

/* Draw floor and ceiling */
void clear_screen();

/* Draw textured walls */
void draw_columns();

int raycast(
		int *wall_i,
		int *side,
		u32 *column_col,
		float angle
		);

float march_by_vertical_grid(
		int *rayX, int *rayY,
		int *wall_i,
		int *side,
		u32 *column_col,
		float angle
		);

float march_by_horizontal_grid(
		int *rayX, int *rayY,
		int *wall_i,
		int *side,
		u32 *column_col,
		float angle
		);

void set_wall_index(char wall, int *wall_i, u32 *column_col);

int get_column_len(int ray_len);

void draw_wall_column(
		char wall,
		int side,
		int column_len,
		u32 column_col,
		int column_x,
		int *column_y
		);

void draw_floor_ceiling_columns(int screen_x, int screen_y, float angle);

/*
========================================================================
						LOADER.C DEFINITIONS
========================================================================
*/

void load_map_file(const char *path);

void load_ppm(const char *path, pixel_t *ppm);

