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
	
	sprintf (buffer_file, "%s%s", systemdata_path, "ccfacefront.ttf");
	button_font = TTF_OpenFont (buffer_file, 40);
	
	if (!button_font) {
		fprintf (stderr,
			_("Failed to load font file 'CC Face Front'\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	/* Random numbers generator */
	srand (SDL_GetTicks ());
	
	return true;
}

int GameEngine::intro (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	Uint32 fondo, button_shade, button_light;
	SDL_Surface *one_player_text, *two_player_text, *one_player_shadow, *two_player_shadow;
	int button_sel;
	
	int big_size;
	int temp;
	
	fondo = SDL_MapRGB (screen->format, 0xa2, 0x76, 0xab);
	SDL_Color morado_c = {.r = 0x6d, .g = 0x41, .b = 0x5f};
	SDL_Color negro_c = {.r = 0, .g = 0, .b = 0};
	
	button_shade = SDL_MapRGB (screen->format, 0xeb, 0xd1, 0x8d);
	button_light = SDL_MapRGB (screen->format, 0xfa, 0xf1, 0xd9);
	
	/* Prerender text for buttons */
	one_player_text = TTF_RenderUTF8_Blended (button_font, _("1 Player"), morado_c);
	one_player_shadow = TTF_RenderUTF8_Blended (button_font, _("1 Player"), negro_c);
	two_player_text = TTF_RenderUTF8_Blended (button_font, _("2 Players"), morado_c);
	two_player_shadow = TTF_RenderUTF8_Blended (button_font, _("2 Players"), negro_c);
	
	big_size = one_player_text->w;
	if (two_player_text->w > big_size) big_size = two_player_text->w;
	
	big_size += 20;
	
	button_sel = 0;
	
	do {
		last_time = SDL_GetTicks ();
		
		SDL_FillRect (screen, NULL, fondo);
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Lets close the app */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					temp = (400 - big_size) / 2;
					if (event.motion.x >= temp && event.motion.x < temp + big_size &&
					    event.motion.y >= 350 && event.motion.y < 400) {
						/* Is on the first button */
						button_sel = 1;
					} else if (event.motion.x >= 400 + temp && event.motion.x < 400 + temp + big_size &&
					    event.motion.y >= 350 && event.motion.y < 400) {
						/* Mouse is over second button */
						button_sel = 2;
					} else {
						button_sel = 0;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.x >= temp && event.button.x < temp + big_size &&
					    event.button.y >= 350 && event.button.y < 400) {
						/* Configure to 1 player */
						players = 1;
						done = GAME_CONTINUE;
					} else if (event.button.x >= 400 + temp && event.button.x < 400 + temp + big_size &&
					    event.button.y >= 350 && event.button.y < 400) {
						/* Mouse is over second button */
						/* Configure to 2 players */
						players = 2;
						done = GAME_CONTINUE;
					}
					break;
				case SDL_KEYDOWN:
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						SDL_WM_ToggleFullScreen (screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					
					break;
				case SDL_KEYUP:
					key = event.key.keysym.sym;
					
					break;
			}
		}
		/* Draw the Logo */
		rect.w = library->images[Library::IMG_LOGO]->w;
		rect.h = library->images[Library::IMG_LOGO]->h;
		
		rect.x = (800 - rect.w) / 2;
		rect.y = 40;
		
		SDL_BlitSurface (library->images[Library::IMG_LOGO], NULL, screen, &rect);
		
		/* Draw the buttons */
		rect.x = (400 - big_size) / 2;
		rect.w = big_size;
		rect.h = 50;
		rect.y = 350;
		
		if (button_sel == 1) {
			SDL_FillRect (screen, &rect, button_light);
		} else {
			SDL_FillRect (screen, &rect, button_shade);
		}
		
		rect.w = one_player_text->w;
		rect.h = one_player_text->h;
		rect.x = (400 - rect.w) / 2 + 2;
		rect.y = 355 + 2;
		
		SDL_BlitSurface (one_player_shadow, NULL, screen, &rect);
		
		rect.w = one_player_text->w;
		rect.h = one_player_text->h;
		rect.x = (400 - rect.w) / 2;
		rect.y = 355;
		
		SDL_BlitSurface (one_player_text, NULL, screen, &rect);
		
		/* Second button */
		rect.x = 400 + (400 - big_size) / 2;
		rect.w = big_size;
		rect.h = 50;
		rect.y = 350;
		
		if (button_sel == 2) {
			SDL_FillRect (screen, &rect, button_light);
		} else {
			SDL_FillRect (screen, &rect, button_shade);
		}
		
		rect.w = two_player_text->w;
		rect.h = two_player_text->h;
		rect.x = 400 + (400 - rect.w) / 2 + 2;
		rect.y = 355 + 2;
		
		SDL_BlitSurface (two_player_shadow, NULL, screen, &rect);
		
		rect.w = two_player_text->w;
		rect.h = two_player_text->h;
		rect.x = 400 + (400 - rect.w) / 2;
		rect.y = 355;
		
		SDL_BlitSurface (two_player_text, NULL, screen, &rect);
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
	} while (!done);
	
	SDL_FreeSurface (one_player_text);
	SDL_FreeSurface (one_player_shadow);
	SDL_FreeSurface (two_player_text);
	SDL_FreeSurface (two_player_shadow);
	return done;
}

int GameEngine::loop (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	SDLKey player_1_keys[5], player_2_keys[5];
	Uint32 fondo;
	
	fondo = SDL_MapRGB (screen->format, 0xa2, 0x76, 0xab);
	
	Map p1;
	Map p2;
	
	if (players == 1) {
		player_1_keys[0] = SDLK_a; /* Rotate clockwise */
		player_1_keys[1] = SDLK_s; /* Rotate counter-clockwise */
		player_1_keys[2] = SDLK_LEFT; /* move left */
		player_1_keys[3] = SDLK_RIGHT; /* move right */
		player_1_keys[4] = SDLK_DOWN; /* Speed piece */
	} else {
		player_1_keys[0] = SDLK_a; /* Rotate clockwise */
		player_1_keys[1] = SDLK_s; /* Rotate counter-clockwise */
		player_1_keys[2] = SDLK_j; /* move left */
		player_1_keys[3] = SDLK_l; /* move right */
		player_1_keys[4] = SDLK_k; /* Speed piece */
		
		player_2_keys[0] = SDLK_KP1; /* Rotate clockwise */
		player_2_keys[1] = SDLK_KP2; /* Rotate counter-clockwise */
		player_2_keys[2] = SDLK_LEFT; /* move left */
		player_2_keys[3] = SDLK_RIGHT; /* move right */
		player_2_keys[4] = SDLK_DOWN; /* Speed piece */
	}
	
	p1.set_origin (40, 80, 1);
	p2.set_origin (544, 80, 2);
	
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
						
					if (key == player_1_keys[0]) {
						p1.send_rotate_clock ();
					} else if (key == player_1_keys[1]) {
						p1.send_rotate_counter ();
					} else if (key == player_1_keys[2]) {
						p1.send_move_left ();
					} else if (key == player_1_keys[3]) {
						p1.send_move_right ();
					} else if (key == player_1_keys[4]) {
						p1.send_down ();
					}
					
					if (players == 2) {
						if (key == player_2_keys[0]) {
							p2.send_rotate_clock ();
						} else if (key == player_2_keys[1]) {
							p2.send_rotate_counter ();
						} else if (key == player_2_keys[2]) {
							p2.send_move_left ();
						} else if (key == player_2_keys[3]) {
							p2.send_move_right ();
						} else if (key == player_2_keys[4]) {
							p2.send_down ();
						}
					}
					break;
				case SDL_KEYUP:
					key = event.key.keysym.sym;
					
					if (key == player_1_keys[4]) {
						p1.send_stop_down ();
					}
					if (players == 2) {
						if (key == player_2_keys[4]) {
							p2.send_stop_down ();
						}
					}
					break;
			}
		}
		
		p1.animate ();
		if (players == 2) {
			p2.animate ();
		}
		p1.draw (screen);
		
		if (players == 2) {
			p2.draw (screen);
		}
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
	} while (!done);
	
	return done;
}

