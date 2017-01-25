/*
 * library.cpp
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

#include <SDL.h>
#include <SDL_image.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "library.h"
#include "path.h"

MediaLibrary *library = NULL;

/* List all images file names */
static const char *images_names[Library::NUM_IMGS] = {
	"images/logo.png",
	"images/bubble.png",
	
	"images/put_1.png",
	"images/put_2.png",
	"images/put_3.png",
	"images/put_4.png",
	"images/put_5.png",
	"images/put_6.png",
	"images/put_7.png",
	"images/put_8.png",
	"images/put_9.png",
	
	"images/pop_1.png",
	"images/pop_2.png",
	
	"images/azul_idle_1.png",
	"images/azul_idle_2.png",
	"images/azul_idle_3.png",
	"images/azul_idle_4.png",
	"images/azul_put.png",
	"images/azul_fall.png",
	
	"images/rojo_idle_1.png",
	"images/rojo_idle_2.png",
	"images/rojo_idle_3.png",
	"images/rojo_idle_4.png",
	"images/rojo_put.png",
	"images/rojo_fall.png",
	
	"images/verde_idle_1.png",
	"images/verde_idle_2.png",
	"images/verde_idle_3.png",
	"images/verde_idle_4.png",
	"images/verde_put.png",
	"images/verde_fall.png",
	
	"images/amarillo_idle_1.png",
	"images/amarillo_idle_2.png",
	"images/amarillo_idle_3.png",
	"images/amarillo_idle_4.png",
	"images/amarillo_put.png",
	"images/amarillo_fall.png"
};
		
int MediaLibrary::load (void) {
	SDL_Surface * image;
	
	int g;
	char buffer_file[8192];
	
	for (g = 0; g < Library::NUM_IMGS; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, images_names[g]);
		image = IMG_Load (buffer_file);
		
		if (image == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n"), buffer_file, SDL_GetError());
			SDL_Quit ();
			return false;
		}
		
		images[g] = image;
		/* TODO: Show loading progress */
	}
	
	return true;
}
