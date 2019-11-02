#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

/*
========================================================================
							MACROS
========================================================================
*/

/* ARGB pixel format */
#define KC_PACK_COLOR(R, G, B) ( ( 255<<24) |( R<<16 ) | ( G<<8 ) | B )

#define DEG2RAD(a) ( M_PI/180 * a )
#define RAD2DEG(a) ( 180/M_PI * a )


/*
========================================================================
							TYPES
========================================================================
*/

struct kc_map_s
{
	/* TODO: invent map type and map format */
	char *grid;
	int block;
	int grid_w, grid_h;
};
typedef struct kc_map_s kc_map_t;

struct kc_player_s
{
	int unit_x, unit_y;
	/* Angles are in degrees*/
	float view_angle;
	float fov;
};
typedef struct kc_player_s kc_player_t;

struct kc_pair_s
{
	char wall;
	SDL_Texture *sdl_text;
};
typedef struct kc_pair_s kc_pair_t;

struct kc_maptext_s
{
	kc_pair_t *data;
	int count;
	int limit;
};
typedef struct kc_maptext_s kc_maptext_t;

/*
========================================================================
						MISCELLANEA
========================================================================
*/

SDL_Texture* loadTexture(char *path, SDL_Renderer *sdl_rend);

void kc_maptext_init(kc_maptext_t *mt);
void kc_maptext_insert(kc_maptext_t *mt, char wall, SDL_Texture *sdl_text);
SDL_Texture* kc_maptext_find(kc_maptext_t *mt, char wall);


/*
========================================================================
						KC_RENDERER DEFINITIONS
========================================================================
*/

/* Main rendering function */
void kc_3d_refresh(
		kc_maptext_t *mt,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player);

/* Draw floor and ceiling */
void kc_clear_screen(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend, 
		kc_map_t *kc_map);

void kc_wall_refresh(
		kc_maptext_t *mt,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player);

/* Main raycasting function */
int kc_raycast(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		kc_map_t* kc_map,
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle);

float kc_verticalgrid_intersection(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		kc_map_t* kc_map,
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle);

float kc_horizontalgrid_intersection(
		int *wall_i,
		int *side,
		SDL_Renderer *sdl_rend,
		kc_map_t* kc_map,
		kc_player_t *kc_player,
		uint32_t *column_col,
		float angle);

int kc_get_column_len(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend, 
		kc_player_t *kc_player_t,
		float angle,
		int ray_len,
		uint32_t *column_col);

void kc_draw_column(
		kc_maptext_t *mt,
		char wall,
		int side,
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		int column_len,
		uint32_t column_col,
		int column_x);


/*
========================================================================
						KC_DEBUG DEFINITIONS
========================================================================
*/
void kc_map_view_update(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player);

/*
void kc_map_view_walls(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player);
*/

void kc_map_view_player(
		SDL_Renderer *sdl_rend,
		kc_map_t *kc_map,
		kc_player_t *kc_player);

void kc_intersect_draw(
		int win_w, int win_h,
		SDL_Renderer *sdl_rend,
		int x, int y,
		uint32_t color);
