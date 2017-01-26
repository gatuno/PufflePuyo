/*
 * falling.h
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

#ifndef __FALLING_H__
#define __FALLING_H__

#include <SDL.h>

enum {
	PIECE_UP,
	PIECE_LEFT,
	PIECE_DOWN,
	PIECE_RIGHT
};

class FallingPiece {
	private:
		int x, y; /* The main part of the piece */
		int p_x, p_y; /* The other part of the piece */
		int color_1, color_2;
		int rotate;
		int offset_y;
		bool acel;
		
		int gem_counter;
	public:
	/* Methods */
		FallingPiece (void);
		
		void reset (void);
		void start_drop (void);
		void draw (SDL_Surface *, int, int);
		
		/* To move the piece */
		void rotate_clock (int map[15][6]);
		void rotate_counter (int map[15][6]);
		void move_left (int map[15][6]);
		void move_right (int map[15][6]);
		
		void fall (void);
		void get_xy (int *x1, int *y1, int *x2, int *y2);
		bool has_falled (void);
		void get_color (int *c1, int *c2);
		void start_acel (void);
		void stop_acel (void);
};


#endif /* __FALLING_H__ */

