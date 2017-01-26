/*
 * msg.h
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

#ifndef __MSG_H__
#define __MSG_H__

#include <SDL.h>

#include "library.h"

#define MAX_MESSAGE 10

class Message {
	private:
		int msg_start, msg_end;
		SDL_Surface *message_images[MAX_MESSAGE];
		int timer[MAX_MESSAGE];
		int offset;
		int pos_x, pos_y;
		void add (char *, SDL_Color);
		
	public:
		Message (void);
		void addChain (int n);
		void addCombo (int n);
		void addAllClear (void);
		void draw (SDL_Surface *);
		void set_pos (int x, int y);
};

#endif /* __MSG_H__ */

