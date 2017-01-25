/*
 * library.h
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

#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include <SDL.h>

namespace Library {
	/* List all images */
	enum {
		IMG_BUBBLE,
		IMG_BUBBLE_PUT_1,
		IMG_BUBBLE_PUT_2,
		IMG_BUBBLE_PUT_3,
		IMG_BUBBLE_PUT_4,
		IMG_BUBBLE_PUT_5,
		IMG_BUBBLE_PUT_6,
		IMG_BUBBLE_PUT_7,
		IMG_BUBBLE_PUT_8,
		IMG_BUBBLE_PUT_9,
		
		IMG_BUBBLE_POP_1,
		IMG_BUBBLE_POP_2,
		
		IMG_PUFFLE_BLUE_IDLE_1,
		IMG_PUFFLE_BLUE_IDLE_2,
		IMG_PUFFLE_BLUE_IDLE_3,
		IMG_PUFFLE_BLUE_IDLE_4,
		IMG_PUFFLE_BLUE_PUT,
		IMG_PUFFLE_BLUE_FALL,
		
		IMG_PUFFLE_RED_IDLE_1,
		IMG_PUFFLE_RED_IDLE_2,
		IMG_PUFFLE_RED_IDLE_3,
		IMG_PUFFLE_RED_IDLE_4,
		IMG_PUFFLE_RED_PUT,
		IMG_PUFFLE_RED_FALL,
		
		IMG_PUFFLE_GREEN_IDLE_1,
		IMG_PUFFLE_GREEN_IDLE_2,
		IMG_PUFFLE_GREEN_IDLE_3,
		IMG_PUFFLE_GREEN_IDLE_4,
		IMG_PUFFLE_GREEN_PUT,
		IMG_PUFFLE_GREEN_FALL,
		
		IMG_PUFFLE_YELLOW_IDLE_1,
		IMG_PUFFLE_YELLOW_IDLE_2,
		IMG_PUFFLE_YELLOW_IDLE_3,
		IMG_PUFFLE_YELLOW_IDLE_4,
		IMG_PUFFLE_YELLOW_PUT,
		IMG_PUFFLE_YELLOW_FALL,
		
		NUM_IMGS
	};
};

class MediaLibrary {
	public:
		SDL_Surface *images[Library::NUM_IMGS];
		
		/* Main loading function */
		int load (void);
	private:
		
};

/* Global for quick access */
extern MediaLibrary *library;

#endif /* __LIBRARY_H__ */

