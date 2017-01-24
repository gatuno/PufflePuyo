/*
 * map.cpp
 * This file is part of Puffle Puyo
 *
 * Copyright (C) 2017 - Félix Arreola Rodríguez
 *
 * Puffle Puyo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Puffle Puyo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Puffle Puyo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "path.h"
#include "library.h"
#include "engine.h"
#include "map.h"
#include "falling.h"

Map::Map (void) {
	/* Clear map */
	memset (map, 0, sizeof (map));
	
	f_p.start_drop ();
}

void Map::draw (SDL_Surface *screen) {
	int g, h;
	SDL_Rect rect;
	
	for (g = 0; g < 12; g++) {
		for (h = 0; h < 6; h++) {
			if (map[g][h] == COLOR_NONE) {
				continue;
			}
			
			rect.x = 20 + h * 38 - 8;
			rect.y = 20 + g * 36 - 8;
			
			rect.w = library->images [Library::IMG_PUFFLE_BLUE_IDLE_1]->w;
			rect.h = library->images [Library::IMG_PUFFLE_BLUE_IDLE_1]->h;
			
			SDL_BlitSurface (library->images [Library::IMG_PUFFLE_BLUE_IDLE_1], NULL, screen, &rect);
			
			/* Draw the bubble */
			rect.x = 20 + h * 38 - 8;
			rect.y = 20 + g * 36 - 8;
			
			rect.w = library->images [Library::IMG_BUBBLE]->w;
			rect.h = library->images [Library::IMG_BUBBLE]->h;
			
			SDL_BlitSurface (library->images [Library::IMG_BUBBLE], NULL, screen, &rect);
		}
	}
	
	/* Draw the falling piece */
	f_p.draw (screen, 20, 20);
}

void Map::send_rotate_clock (void) {
	f_p.rotate_clock ();
}

void Map::send_rotate_counter (void) {
	f_p.rotate_counter ();
}

void Map::send_move_left (void) {
	f_p.move_left ();
}

void Map::send_move_right (void) {
	f_p.move_right ();
}

void Map::animate (void) {
	f_p.fall ();
}
