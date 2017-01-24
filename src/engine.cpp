/*
 * engine.c
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

#define FPS (1000/24)

bool GameEngine::setup (void) {
	bool loaded;
	int depth;
	char buffer_file[8192];
	SDL_Surface *image;
	
	/* Setup SDL basics */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the video subsystem\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		return false;
	}
	
	sprintf (buffer_file, "%simages/icon.png", systemdata_path);
	image = IMG_Load (buffer_file);
	if (image) {
		SDL_WM_SetIcon (image, NULL);
		SDL_FreeSurface (image);
	}
	SDL_WM_SetCaption (_("Puffle Puyo"), _("Puffle Puyo"));
	
	/* Create window surface */
	depth = SDL_VideoModeOK (800, 600, 16, 0);
	screen = depth ? SDL_SetVideoMode (800, 600, depth, 0) : NULL;
	
	if (screen == NULL) {
		fprintf (stderr,
			_("Error: Can't setup 800x600 video mode.\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		return false;
	}
	
	library = new MediaLibrary ();
	loaded = library->load ();
	
	if (loaded == false) { /* Something gone wrong */
		return false;
	}
	
	if (TTF_Init () < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the SDL TTF library\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		return false;
	}
	
	/* Random numbers generator */
	srand (SDL_GetTicks ());
	
	return true;
}

int GameEngine::loop (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	
	Map p1;
	
	do {
		last_time = SDL_GetTicks ();
		
		SDL_FillRect (screen, NULL, 0);
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Lets close the app */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEBUTTONDOWN:
					
					break;
				case SDL_MOUSEBUTTONUP:
					
					break;
				case SDL_KEYDOWN:
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						SDL_WM_ToggleFullScreen (screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					
					if (key == SDLK_a) {
						p1.send_rotate_clock ();
					} else if (key == SDLK_s) {
						p1.send_rotate_counter ();
					} else if (key == SDLK_LEFT) {
						p1.send_move_left ();
					} else if (key == SDLK_RIGHT) {
						p1.send_move_right ();
					}
					break;
			}
		}
		
		p1.animate ();
		p1.draw (screen);
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}

