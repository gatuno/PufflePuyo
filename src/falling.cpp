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
	p_x = -1;
	p_y = -1;
	acel = false;
	
	color_1 = COLOR_1 + RANDOM (4);
	color_2 = COLOR_1 + RANDOM (4);
	
	//color_1 = COLOR_1;
	//color_2 = COLOR_2;
	
	rotate = PIECE_UP;
	offset_y = 0;
}

void FallingPiece::start_drop (void) {
	x = 3;
	y = 11;
	
	p_x = 3;
	p_y = 12;
	
	offset_y = -36;
}

void FallingPiece::draw (SDL_Surface *screen, int map_x, int map_y) {
	int g, h;
	SDL_Rect rect;
	int image;
	
	/* Draw a highlight for the falling piece
	rect.x = map_x + x * 38 - 4;
	rect.y = map_y + (11 - y) * 36 - 4;
	rect.w = 38;
	rect.h = 36;
	Uint32 azul = SDL_MapRGB (screen->format, 174, 159, 200);
	
	SDL_FillRect (screen, &rect, azul);*/
	
	/* First piece */
	rect.x = map_x + x * 38 - 8;
	rect.y = map_y + (11 - y) * 36 - 8;
	rect.y += offset_y;
	
	if (color_1 == COLOR_1) {
		image = Library::IMG_PUFFLE_BLUE_IDLE_1;
	} else if (color_1 == COLOR_2) {
		image = Library::IMG_PUFFLE_RED_IDLE_1;
	} else if (color_1 == COLOR_3) {
		image = Library::IMG_PUFFLE_GREEN_IDLE_1;
	} else if (color_1 == COLOR_4) {
		image = Library::IMG_PUFFLE_YELLOW_IDLE_1;
	}
	
	rect.w = library->images [image]->w;
	rect.h = library->images [image]->h;
	
	SDL_BlitSurface (library->images [image], NULL, screen, &rect);
	
	/* Draw the bubble */
	rect.x = map_x + x * 38 - 8;
	rect.y = map_y + (11 - y) * 36 - 8;
	rect.y += offset_y;
	
	rect.w = library->images [Library::IMG_BUBBLE]->w;
	rect.h = library->images [Library::IMG_BUBBLE]->h;
	
	SDL_BlitSurface (library->images [Library::IMG_BUBBLE], NULL, screen, &rect);
	
	/* Draw the second piece */
	rect.x = map_x + p_x * 38 - 8;
	rect.y = map_y + (11 - p_y) * 36 - 8;
	rect.y += offset_y;
	
	if (color_2 == COLOR_1) {
		image = Library::IMG_PUFFLE_BLUE_IDLE_1;
	} else if (color_2 == COLOR_2) {
		image = Library::IMG_PUFFLE_RED_IDLE_1;
	} else if (color_2 == COLOR_3) {
		image = Library::IMG_PUFFLE_GREEN_IDLE_1;
	} else if (color_2 == COLOR_4) {
		image = Library::IMG_PUFFLE_YELLOW_IDLE_1;
	}
	
	rect.w = library->images [image]->w;
	rect.h = library->images [image]->h;
	
	SDL_BlitSurface (library->images [image], NULL, screen, &rect);
	
	/* Draw the bubble */
	rect.x = map_x + p_x * 38 - 8;
	rect.y = map_y + (11 - p_y) * 36 - 8;
	rect.y += offset_y;
	
	rect.w = library->images [Library::IMG_BUBBLE]->w;
	rect.h = library->images [Library::IMG_BUBBLE]->h;
	
	SDL_BlitSurface (library->images [Library::IMG_BUBBLE], NULL, screen, &rect);
}

void FallingPiece::rotate_clock (int map[15][6]) {
	int right_block = false, left_block = false;
	int g, h;
	
	/* If the piece is in the right edge, move to the left */
	if (x == 5) {
		right_block = true;
		//printf ("Bloqueado a la derecha por borde\n");
	} else if (x == 0) {
		left_block = true;
		//printf ("Bloqueado a la izquierda por borde\n");
	}
	if (rotate == PIECE_UP || rotate == PIECE_DOWN) {
		if (x < 5 && map[y][x + 1] != COLOR_NONE) {
			right_block = true;
			//printf ("Bloqueado a la derecha por pieza\n");
		}
		if (x > 0 && map[y][x - 1] != COLOR_NONE) {
			left_block = true;
			//printf ("Bloqueado a la izquierda por pieza\n");
		}
	}
	
	if (right_block && left_block) {
		//printf ("Bloqueado por ambos lados\n");
		/* We can't rotate left or right, so we flip vertically */
		g = x;
		h = y;
		x = p_x;
		y = p_y;
		p_x = g;
		p_y = h;
		
		if (rotate == PIECE_DOWN) {
			rotate = PIECE_UP;
		} else {
			rotate = PIECE_DOWN;
		}
		return;
	}
	
	if (right_block && rotate == PIECE_UP) {
		x--;
	}
	
	if (left_block && rotate == PIECE_DOWN) {
		x++;
	}
	
	if (rotate == PIECE_RIGHT) {
		/* As we are rotating down, check if we have collision with the piece below */
		if (y - 1 < 0 || map[y - 1][x] != COLOR_NONE) {
			rotate = PIECE_DOWN;
			offset_y = 0;
			y++;
			p_x = x;
			p_y = y - 1;
			return;
		}
	}
	
	if (rotate == PIECE_UP) {
		rotate = PIECE_RIGHT;
		p_x = x + 1;
		p_y = y;
	} else if (rotate == PIECE_RIGHT) {
		rotate = PIECE_DOWN;
		p_x = x;
		p_y = y - 1;
	} else if (rotate == PIECE_DOWN) {
		rotate = PIECE_LEFT;
		p_x = x - 1;
		p_y = y;
	} else if (rotate == PIECE_LEFT) {
		rotate = PIECE_UP;
		p_x = x;
		p_y = y + 1;
	}
}

void FallingPiece::rotate_counter (int map[15][6]) {
	int right_block = false, left_block = false;
	int g, h;
	
	/* If the piece is in the right edge, move to the left */
	if (x == 5) {
		right_block = true;
		//printf ("Bloqueado a la derecha por borde\n");
	} else if (x == 0) {
		left_block = true;
		//printf ("Bloqueado a la izquierda por borde\n");
	}
	if (rotate == PIECE_UP || rotate == PIECE_DOWN) {
		if (x < 5 && map[y][x + 1] != COLOR_NONE) {
			right_block = true;
			//printf ("Bloqueado a la derecha por pieza\n");
		}
		if (x > 0 && map[y][x - 1] != COLOR_NONE) {
			left_block = true;
			//printf ("Bloqueado a la izquierda por pieza\n");
		}
	}
	
	if (right_block && left_block) {
		//printf ("Bloqueado por ambos lados\n");
		/* We can't rotate left or right, so we flip vertically */
		g = x;
		h = y;
		x = p_x;
		y = p_y;
		p_x = g;
		p_y = h;
		
		if (rotate == PIECE_DOWN) {
			rotate = PIECE_UP;
		} else {
			rotate = PIECE_DOWN;
		}
		return;
	}
	
	if (left_block && rotate == PIECE_UP) {
		x++;
	}
	
	if (right_block && rotate == PIECE_DOWN) {
		x--;
	}
	
	if (rotate == PIECE_LEFT) {
		/* As we are rotating down, check if we have collision with the piece below */
		if (y - 1 < 0 || map[y - 1][x] != COLOR_NONE) {
			rotate = PIECE_DOWN;
			offset_y = 0;
			y++;
			p_x = x;
			p_y = y - 1;
			return;
		}
	}
	
	if (rotate == PIECE_UP) {
		rotate = PIECE_LEFT;
		p_x = x - 1;
		p_y = y;
	} else if (rotate == PIECE_LEFT) {
		rotate = PIECE_DOWN;
		p_x = x;
		p_y = y - 1;
	} else if (rotate == PIECE_DOWN) {
		rotate = PIECE_RIGHT;
		p_x = x + 1;
		p_y = y;
	} else if (rotate == PIECE_RIGHT) {
		rotate = PIECE_UP;
		p_x = x;
		p_y = y + 1;
	}
}

void FallingPiece::move_left (int map[15][6]) {
	if (x > 0) {
		if (map[y][x - 1] != COLOR_NONE) {
			return; /* Cant move to the left */
		}
	}
	if (p_x > 0 && p_y > 0) {
		if (map[p_y][p_x - 1] != COLOR_NONE) {
			return; /* The other part of the falling piece can't move to the left */
		}
	}
	
	if (rotate == PIECE_LEFT) {
		if (x > 1) {
			x--;
			p_x--;
		}
	} else if (x > 0) {
		x--;
		p_x--;
	}
}

void FallingPiece::move_right (int map[15][6]) {
	if (x < 5) {
		if (map[y][x + 1] != COLOR_NONE) {
			return; /* Can't move to the right */
		}
	}
	if (p_x < 5 && p_y > 0) {
		if (map[p_y][p_x + 1] != COLOR_NONE) {
			return;
		}
	}
	if (rotate == PIECE_RIGHT) {
		if (x < 4) {
			x++;
			p_x++;
		}
	} else if (x < 5) {
		x++;
		p_x++;
	}
}

void FallingPiece::fall (void) {
	if (acel) {
		if (offset_y % 4 != 0) {
			//printf ("%i Mod 4: %i\n", offset_y, (offset_y % 4));
			offset_y += (-1 * (offset_y % 4));
			//printf ("%i after\n", offset_y);
		} else {
			offset_y += 4;
		}
	} else {
		offset_y++;
	}
	
	if (offset_y > 0) {
		y--;
		p_y--;
		if (acel) {
			offset_y = -32;
		} else {
			offset_y = -35;
		}
	}
}

void FallingPiece::get_xy (int *x1, int *y1, int *x2, int *y2) {
	if (x1 != NULL) {
		*x1 = x;
	}
	if (y1 != NULL) {
		*y1 = y;
	}
	
	if (x2 != NULL) {
		*x2 = p_x;
	}
	if (y2 != NULL) {
		*y2 = p_y;
	}
}

bool FallingPiece::has_falled (void) {
	if (offset_y == 0) {
		return true;
	}
	
	return false;
}

void FallingPiece::get_color (int *c1, int *c2) {
	if (c1 != NULL) {
		*c1 = color_1;
	}
	
	if (c2 != NULL) {
		*c2 = color_2;
	}
}

void FallingPiece::start_acel (void) {
	acel = true;
}

void FallingPiece::stop_acel (void) {
	acel = false;
}

