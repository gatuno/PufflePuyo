/*
 * msg.cpp
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

#include <cstdio>

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "msg.h"
#include "library.h"

/* Static methods */
static void fade_out_text (SDL_Surface *text, int alpha) {
	int x, y;
	Uint8 *p;
	int bpp;
	Uint32 color;
	Uint8 r, g, b, a;
	
	if (alpha >= 256) alpha = 255;
	if (alpha < 0) alpha = 0;
	
	bpp = text->format->BytesPerPixel;
	
	for (y = 0; y < text->h; y++) {
		for (x = 0; x < text->w; x++) {
			p = (Uint8 *)text->pixels + y * text->pitch + x * bpp;
			
			color = *((Uint32 *)p);
			
			SDL_GetRGBA (color, text->format, &r, &g, &b, &a);
			if (alpha > a) continue;
			a = alpha;
			color = SDL_MapRGBA (text->format, r, g, b, a);
			
			*((Uint32 *)p) = color;
		}
	}
}

Message::Message (void) {
	msg_start = msg_end = 0;
	offset = -1;
	pos_x = pos_y = 0;
}

void Message::draw (SDL_Surface *screen) {
	int g, h;
	int start;
	SDL_Rect rect;
	int pos;
	
	if (msg_end < msg_start) {
		h = msg_end + MAX_MESSAGE - msg_start;
	} else {
		h = msg_end - msg_start;
	}
	
	if (h == 0) return;
	
	if (offset >= 0) {
		offset += 5;
		if (offset == 30) {
			/* There is enough space for the new message */
			start = msg_end;
			
			offset = -1;
		} else {
			/* Start from the previous message */
			start = msg_end - 1;
		}
	} else {
		/* There is no pending message */
		start = msg_end;
	}
	if (start < 0) start += MAX_MESSAGE;
	
	/* Iterate over all messages in reverse order */
	g = start;
	h = 0;
	while (g != msg_start) {
		pos = g - 1;
		if (pos < 0) pos += MAX_MESSAGE;
		
		rect.y = pos_y + h + (offset >= 0 ? offset : 0);
		rect.w = message_images[pos]->w;
		rect.h = message_images[pos]->h;
		rect.x = pos_x - (rect.w / 2);
		
		timer[pos]++;
		
		if (timer[pos] >= 112) {
			/* Fade out the text */
			fade_out_text (message_images[pos], (120 - timer[pos]) * 32);
		}
		
		SDL_BlitSurface (message_images[pos], NULL, screen, &rect);
		
		if (timer[pos] >= 120 && pos == msg_start) {
			/* Erase the last message */
			SDL_FreeSurface (message_images[msg_start]);
			msg_start = (msg_start + 1) % MAX_MESSAGE;
			break;
		}
		
		h = h + 30;
		g = pos;
	}
}

void Message::addChain (int n) {
	char buffer[256];
	SDL_Color rojo;
	
	rojo.r = 0xee; rojo.g = 0x44; rojo.b = 0x44;
	
	sprintf (buffer, _("%i Chain!"), n);
	
	add (buffer, rojo);
}

void Message::addCombo (int n) {
	char buffer[256];
	SDL_Color azul;
	
	azul.r = 0; azul.g = 0xcc; azul.b = 0xff;
	sprintf (buffer, _("%i Combo!"), n);
	
	add (buffer, azul);
}

void Message::addAllClear (void) {
	SDL_Color am;
	
	am.r = 0xff; am.g = 0xff; am.b = 0;
	add (_("All clear!"), am);
}

void Message::add (char *buffer, SDL_Color c) {
	int g;
	
	if (msg_end < msg_start) {
		g = msg_end + MAX_MESSAGE - msg_start;
	} else {
		g = msg_end - msg_start;
	}
	
	if (g > 0) {
		offset = 0;
	}
	/* If the list is full, erase the last on */
	if (msg_start == (msg_end + 1) % MAX_MESSAGE) {
		SDL_FreeSurface (message_images[msg_start]);
		msg_start = (msg_start + 1) % MAX_MESSAGE;
	}
	
	message_images[msg_end] = TTF_RenderUTF8_Blended (library->points_font, buffer, c);
	timer[msg_end] = 0;
	
	msg_end = (msg_end + 1) % MAX_MESSAGE;
}

void Message::set_pos (int x, int y) {
	pos_x = x;
	pos_y = y;
}
