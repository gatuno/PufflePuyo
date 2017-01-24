/*
 * map.h
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

#ifndef __MAP_H__
#define __MAP_H__

#include <SDL.h>

#include "falling.h"

enum {
	COLOR_NONE = 0,
	COLOR_1,
	COLOR_2,
	COLOR_3,
	COLOR_4
};

class Map {
	private:
		int map[12][6];
		
		FallingPiece f_p;
	public:
		Map (void);
		
		void draw (SDL_Surface *);
		void send_rotate_clock (void);
		void send_rotate_counter (void);
		void send_move_left (void);
		void send_move_right (void);
		
		void animate (void);
};

#endif /* __MAP_H__ */

