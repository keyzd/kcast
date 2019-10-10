#include "kcast.h"

void kc_3d_refresh(kc_screen_t *kc_screen)
{
	kc_clear_screen(kc_screen);
	kc_wall_refresh(kc_screen);
}

void kc_clear_screen(kc_screen_t *kc_screen)
{
	int i;

	for(i = 0; i < kc_screen->w * kc_screen->h; i++)
	{
		kc_screen->pixels[i] = KC_PACK_COLOR(0, 0, 0);
	}
}

void kc_wall_refresh(kc_screen_t *kc_screen)
{
}
