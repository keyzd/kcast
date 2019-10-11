#include <stdint.h>

/*
========================================================================
							MACROS
========================================================================
*/

/* ARGB pixel format */
#define KC_PACK_COLOR(R, G, B) ( ( 255<<24) |( R<<16 ) | ( G<<8 ) | B )

#define DEG2RAD(a) ( 3.14159265/180 * a )
#define RAD2DEG(a) ( 180/3.14159265 * a )


/*
========================================================================
							TYPES
========================================================================
*/

struct kc_screen_s
{
	uint32_t *pixels;
	int w, h;
};
typedef struct kc_screen_s kc_screen_t;

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


/*
========================================================================
						KC_RENDERER DEFINITIONS
========================================================================
*/

void kc_3d_refresh(kc_screen_t *kc_screen, kc_map_t *kc_map, kc_player_t *kc_player); /* Main rendering function */
void kc_clear_screen(kc_screen_t *kc_screen, kc_map_t *kc_map); /* Draw floor and ceiling */
void kc_wall_refresh(kc_screen_t *kc_screen, kc_map_t *kc_map, kc_player_t *kc_player);
int kc_raycast(kc_map_t* kc_map, kc_player_t *kc_player, uint32_t *column_col); /* Get ray lenght */
int kc_get_column_len(kc_screen_t *kc_screen, int ray_len, uint32_t *column_col);
void kc_draw_column(kc_screen_t *kc_screen, int column_len, uint32_t column_col, int column_x);
