/*
 * puyo.cpp
 * This file is part of Puffle Puyo
 *
 * Copyright (C) 2011 - Félix Arreola Rodríguez
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

#define FPS (1000/24)

int main (int argc, char *argv[]) {
	/* Recover system data paths */
	initSystemPaths (argv[0]);
	
	/* Initialize l18n */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, l10n_path);
	
	textdomain (PACKAGE);
	
	GameEngine engine;
	bool loaded;
	loaded = engine.setup ();
	
	if (loaded == false) {
		/* Can't load something, exit */
		return EXIT_FAILURE;
	}
	
	do {
		if (engine.intro () == GAME_QUIT) break;
		if (engine.loop () == GAME_QUIT) break;
		//if (engine.finish () == GAME_QUIT) break;
	} while (1 == 0);
	
	SDL_Quit ();
	return EXIT_SUCCESS;
}

