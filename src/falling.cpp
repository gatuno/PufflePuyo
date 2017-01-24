/*
 * falling.cpp
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

#include <cstdlib>

#include <SDL.h>

#include "falling.h"
#include "map.h"
#include "library.h"

#define RANDOM(x) ((int) (x ## .0 * rand () / (RAND_MAX + 1.0)))

FallingPiece::FallingPiece (void) {
	reset ();
}

void FallingPiece::reset (void) {
	x = -1;
	y = -1;
	
	color_1 = COLOR_1 + RANDOM (4);
	color_2 = COLOR_1 + RANDOM (4);
	
	color_1 = COLOR_1;
	color_2 = COLOR_2;
	
	rotate = PIECE_UP;
	offset_y = 0;
}

void FallingPiece::start_drop (void) {
	x = 3;
	y = 2;
	
	offset_y = -36;
}

void FallingPiece::draw (SDL_Surface *screen, int map_x, int map_y) {
	int g, h;
	SDL_Rect rect;
	int image;
	
	/* First piece */
	rect.x = map_x + x * 38 - 8;
	rect.y = map_y + y * 36 - 8;
	rect.y += offset_y;
	
	if (color_1 == COLOR_1) {
		image = Library::IMG_PUFFLE_BLUE_IDLE_1;
	} else if (color_1 == COLOR_2) {
		image = Library::IMG_PUFFLE_RED_IDLE_1;
	}
	
	rect.w = library->images [image]->w;
	rect.h = library->images [image]->h;
	
	SDL_BlitSurface (library->images [image], NULL, screen, &rect);
	
	/* Draw the bubble */
	rect.x = map_x + x * 38 - 8;
	rect.y = map_y + y * 36 - 8;
	rect.y += offset_y;
	
	rect.w = library->images [Library::IMG_BUBBLE]->w;
	rect.h = library->images [Library::IMG_BUBBLE]->h;
	
	SDL_BlitSurface (library->images [Library::IMG_BUBBLE], NULL, screen, &rect);
	
	if (rotate == PIECE_UP) {
		g = 0; h = -1;
	} else if (rotate == PIECE_DOWN) {
		g = 0; h = 1;
	} else if (rotate == PIECE_LEFT) {
		g = -1; h = 0;
	} else if (rotate == PIECE_RIGHT) {
		g = 1; h = 0;
	}
	
	/* Draw the second piece */
	rect.x = map_x + (x + g) * 38 - 8;
	rect.y = map_y + (y + h) * 36 - 8;
	rect.y += offset_y;
	
	if (color_2 == COLOR_1) {
		image = Library::IMG_PUFFLE_BLUE_IDLE_1;
	} else if (color_2 == COLOR_2) {
		image = Library::IMG_PUFFLE_RED_IDLE_1;
	}
	
	rect.w = library->images [image]->w;
	rect.h = library->images [image]->h;
	
	SDL_BlitSurface (library->images [image], NULL, screen, &rect);
	
	/* Draw the bubble */
	rect.x = map_x + (x + g) * 38 - 8;
	rect.y = map_y + (y + h) * 36 - 8;
	rect.y += offset_y;
	
	rect.w = library->images [Library::IMG_BUBBLE]->w;
	rect.h = library->images [Library::IMG_BUBBLE]->h;
	
	SDL_BlitSurface (library->images [Library::IMG_BUBBLE], NULL, screen, &rect);
}

void FallingPiece::rotate_clock (void) {
	/* If the piece is in the right edge, move to the left */
	if (x == 5 && rotate == PIECE_UP) {
		x = 4;
	} else if (x == 0 && rotate == PIECE_DOWN) {
		x = 1;
	}
	
	if (rotate == PIECE_UP) {
		rotate = PIECE_RIGHT;
	} else if (rotate == PIECE_RIGHT) {
		rotate = PIECE_DOWN;
	} else if (rotate == PIECE_DOWN) {
		rotate = PIECE_LEFT;
	} else if (rotate == PIECE_LEFT) {
		rotate = PIECE_UP;
	}
}

void FallingPiece::rotate_counter (void) {
	/* If the piece is in the right edge, move to the left */
	if (x == 5 && rotate == PIECE_DOWN) {
		x = 4;
	} else if (x == 0 && rotate == PIECE_UP) {
		x = 1;
	}
	
	if (rotate == PIECE_UP) {
		rotate = PIECE_LEFT;
	} else if (rotate == PIECE_LEFT) {
		rotate = PIECE_DOWN;
	} else if (rotate == PIECE_DOWN) {
		rotate = PIECE_RIGHT;
	} else if (rotate == PIECE_RIGHT) {
		rotate = PIECE_UP;
	}
}

void FallingPiece::move_left (void) {
	if (rotate == PIECE_LEFT) {
		if (x > 1) {
			x--;
		}
	} else if (x > 0) {
		x--;
	}
}

void FallingPiece::move_right (void) {
	if (rotate == PIECE_RIGHT) {
		if (x < 4) {
			x++;
		}
	} else if (x < 5) {
		x++;
	}
}

void FallingPiece::fall (void) {
	offset_y++;
	
	if (offset_y == 0) {
		y++;
		offset_y = -36;
	}
}
