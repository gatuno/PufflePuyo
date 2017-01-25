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
#include "sdl_point.h"

Map::Map (void) {
	/* Clear map */
	memset (map, 0, sizeof (map));
	memset (falling_offsets, 0, sizeof (falling_offsets));
	
	pos_x = 40;
	pos_y = 80;
	
	f_p.start_drop ();
	animating = MAP_ANIMATE_NONE;
}

void Map::draw (SDL_Surface *screen) {
	int g, h;
	SDL_Rect rect;
	int image;
	
	Uint32 blanco = SDL_MapRGB (screen->format, 255, 255, 255);
	
	/* Draw some guide lines */
	for (h = 0; h < 7; h++) {
		rect.x = pos_x + (h * 38) - 4;
		rect.w = 1;
		
		rect.y = pos_y - 4;
		rect.h = (12 * 36);
		
		SDL_FillRect (screen, &rect, blanco);
	}
	
	for (g = 0; g < 13; g++) {
		rect.y = pos_y + (g * 36) - 4;
		rect.h = 1;
		
		rect.x = pos_x - 4;
		rect.w = (6 * 38);
		
		SDL_FillRect (screen, &rect, blanco);
	}
	
	for (g = 0; g < 12; g++) {
		for (h = 0; h < 6; h++) {
			if (map[g][h] == COLOR_NONE) {
				continue;
			}
			
			rect.x = pos_x + h * 38 - 8;
			rect.y = pos_y + g * 36 - 8;
			
			if (falling_offsets[g][h] > 0) {
				rect.y = rect.y - falling_offsets[g][h];
			}
			if (map[g][h] == COLOR_1) {
				image = Library::IMG_PUFFLE_BLUE_IDLE_1;
			} else if (map[g][h] == COLOR_2) {
				image = Library::IMG_PUFFLE_RED_IDLE_1;
			}
			rect.w = library->images [image]->w;
			rect.h = library->images [image]->h;
			
			SDL_BlitSurface (library->images [image], NULL, screen, &rect);
			
			/* Draw the bubble */
			rect.x = pos_x + h * 38 - 8;
			rect.y = pos_y + g * 36 - 8;
			if (falling_offsets[g][h] > 0) {
				rect.y = rect.y - falling_offsets[g][h];
			}
			
			rect.w = library->images [Library::IMG_BUBBLE]->w;
			rect.h = library->images [Library::IMG_BUBBLE]->h;
			
			SDL_BlitSurface (library->images [Library::IMG_BUBBLE], NULL, screen, &rect);
		}
	}
	
	/* Draw the falling piece */
	if (animating == MAP_ANIMATE_NONE) {
		f_p.draw (screen, pos_x, pos_y);
	}
}

void Map::send_rotate_clock (void) {
	f_p.rotate_clock (map);
}

void Map::send_rotate_counter (void) {
	f_p.rotate_counter (map);
}

void Map::send_move_left (void) {
	if (animating == MAP_ANIMATE_NONE) {
		/* Validation is made inside the falling piece */
		f_p.move_left (map);
	}
}

void Map::send_move_right (void) {
	if (animating == MAP_ANIMATE_NONE) {
		/* Validation is made inside the falling piece */
		f_p.move_right (map);
	}
}

void Map::send_down (void) {
	if (animating == MAP_ANIMATE_NONE) {
		f_p.start_acel ();
	}
}

void Map::send_stop_down (void) {
	if (animating == MAP_ANIMATE_NONE) {
		f_p.stop_acel ();
	}
}

void Map::animate (void) {
	int x1, x2, y1, y2;
	int color_1, color_2;
	int g, h;
	bool new_piece;
	
	new_piece = false;
	if (animating == MAP_ANIMATE_NONE) {
		/* Check if the falling piece is at a boundary */
		if (f_p.has_falled ()) {
			/* Check now for collissions */
			f_p.get_xy (&x1, &y1, &x2, &y2);
			f_p.get_color (&color_1, &color_2);
		
			if (y1 == 11 || y2 == 11) {
				/* Bottom, stop the piece */
				/* Put the Piece on the map */
				map[y1][x1] = color_1;
				map[y2][x2] = color_2;
				
				new_piece = true;
				f_p.reset ();
			} else if (map[y1 + 1][x1] != COLOR_NONE) {
				/* Colission */
				map[y1][x1] = color_1;
				
				/* Check how much the other part will fall */
				for (g = y2; g < 11; g++) {
					if (map[g + 1][x2] != COLOR_NONE) {
						/* Stop here please */
						break;
					}
				}
				
				map[g][x2] = color_2;
				new_piece = true;
				
				f_p.reset ();
				if (g - y2 != 0) {
					falling_offsets[g][x2] = (g - y2) * 36;
					animating = MAP_ANIMATE_FALLING;
				}
			} else if (map[y2 + 1][x2] != COLOR_NONE) {
				/* Colission */
				f_p.get_color (&color_1, &color_2);
				
				map[y2][x2] = color_2;
				
				/* Check how much the other part will fall */
				for (g = y1; g < 11; g++) {
					if (map[g + 1][x1] != COLOR_NONE) {
						/* Stop here please */
						break;
					}
				}
				
				map[g][x1] = color_1;
				new_piece = true;
				f_p.reset ();
				if (g - y1 != 0) {
					falling_offsets[g][x1] = (g - y1) * 36;
					animating = MAP_ANIMATE_FALLING;
				}
			} else {
				f_p.fall ();
			}
		} else {
			f_p.fall ();
		}
	} else if (animating == MAP_ANIMATE_FALLING) {
		/* Decrement offsets to make pieces fall slowy */
		bool still = false;
		for (g = 0; g < 12; g++) {
			for (h = 0; h < 6; h++) {
				if (falling_offsets[g][h] != 0) {
					falling_offsets[g][h] -= 4;
					
					if (falling_offsets[g][h] > 0) still = true;
				}
			}
		}
		
		if (still == false) {
			/* Send another piece */
			new_piece = true;
			//f_p.start_drop ();
			animating = MAP_ANIMATE_NONE;
		}
	}
	
	if (new_piece) {
		/* Check for pieces that may be together */
		check_islands ();
		
		/* If nothing has been erased, then send the next piece */
		if (animating == MAP_ANIMATE_NONE) {
			f_p.start_drop ();
		}
	}
}

void Map::check_islands (void) {
	int visitados[12][6];
	int x, y, g, h;
	int i;
	
	/* Max island should never be more than 12, I hope */
	SDLPoint next[12];
	int color;
	int size;
	memset (visitados, 0, sizeof (visitados));
	
	for (y = 0; y < 12; y++) {
		for (x = 0; x < 6; x++) {
			if (visitados[y][x] == 1) continue;
			
			if (map[y][x] == COLOR_NONE) {
				visitados[y][x] = 1;
				continue;
			}
			
			size = 1;
			next[0].x = x;
			next[0].y = y;
			color = map[y][x];
			visitados[y][x] = 1;
			
			i = 0;
			while (i < size) {
				g = next[i].x;
				h = next[i].y;
				
				if (h > 0 && map[h - 1][g] == color && visitados[h - 1][g] == 0) {
					/* Add to the next list */
					next[size].x = g;
					next[size].y = h - 1;
					visitados[h - 1][g] = 1;
					size++;
				}
				if (h < 11 && map[h + 1][g] == color && visitados[h + 1][g] == 0) {
					next[size].x = g;
					next[size].y = h + 1;
					visitados[h + 1][g] = 1;
					size++;
				}
				if (g > 0 && map[h][g - 1] == color && visitados[h][g - 1] == 0) {
					next[size].x = g - 1;
					next[size].y = h;
					visitados[h][g - 1] = 1;
					size++;
				}
				if (g < 5 && map[h][g + 1] == color && visitados[h][g + 1] == 0) {
					next[size].x = g + 1;
					next[size].y = h;
					visitados[h][g + 1] = 1;
					size++;
				}
				i++;
			}
			
			if (size >= 4) {
				/* We have a 4 group */
				printf ("We have a group of four:\n");
				for (i = 0; i < size; i++) {
					printf ("Pos: %i, %i\n", next[i].x, next[i].y);
				}
			}
		}
	}
}
