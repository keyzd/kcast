#include <stdint.h>

/*
========================================================================
							MACROS
========================================================================
*/

/* ARGB pixels format */
#define KC_PACK_COLOR(R, G, B) ((255<<24)|(R<<16)|(G<<8)|B)


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

struct kc_player_s
{
	int grid_x, grid_y;
	int unit_x, unit_y;
	float view_angle;
	float fov;
};
typedef struct kc_player_s kc_player_t;


/*
========================================================================
						KC_RENDERER DEFINITIONS
========================================================================
*/

void kc_3d_refresh(kc_screen_t *kc_screen);
void kc_clear_screen(kc_screen_t *kc_screen);
void kc_wall_refresh(kc_screen_t *kc_screen);
