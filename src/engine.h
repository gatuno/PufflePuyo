/*
 * engine.h
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

#ifndef __ENGINE_H__
#define __ENGINE_H__

/* Main exit codes */
enum {
	GAME_NONE = 0, /* Not used */
	GAME_CONTINUE,
	GAME_QUIT
};

class GameEngine {
	private:
		SDL_Surface *screen;
		
	public:
		bool setup (void);
		
		int loop (void);
};

#endif /* __ENGINE_H__ */

