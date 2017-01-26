/*
 * map.cpp
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
#include <string.h>

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
#include "falling.h"
#include "sdl_point.h"
#include "msg.h"

#define RANDOM(x) ((int) (x ## .0 * rand () / (RAND_MAX + 1.0)))

Map::Map (void) {
	/* Clear map */
	memset (map, 0, sizeof (map));
	memset (falling_offsets, 0, sizeof (falling_offsets));
	memset (map_frames, 0, sizeof (map_frames));
	
	pos_x = 0;
	pos_y = 0;
	
	f_p.start_drop ();
	animating = MAP_ANIMATE_NONE;
	
	/* We use a circular list for the poped bubbles */
	poped_start = 0;
	poped_end = 0;
	chain = 0;
	
	/*map[0][0] = map[0][1] = map[1][1] = COLOR_1;
	map[3][1] = map[2][1] = map[1][2] = map[0][2] = map[0][3] = COLOR_2;
	map[1][3] = map[0][4] = map[0][5] = map[1][5] = COLOR_3;
	map[2][2] = map[2][3] = map[1][4] = map[2][5] = COLOR_4;
	map[4][1] = map[3][2] = map[3][3] = map[2][4] = COLOR_1;*/
}

/* This function sets the start corner of this map */
void Map::set_origin (int x, int y, int player) {
	pos_x = x;
	pos_y = y;
	this->player = player;
	
	msgs.set_pos (pos_x + (36 * 6) / 2, pos_y + 10);
}

/* Main drawing function of the map */
void Map::draw (SDL_Surface *screen) {
	int g, h;
	SDL_Rect rect;
	int image;
	
	Uint32 blanco = SDL_MapRGB (screen->format, 32, 32, 32);
	
	/* Draw some guide lines */
	for (h = 0; h < 7; h++) {
		rect.x = pos_x + (h * 38) - 4;
		rect.w = 1;
		
		rect.y = pos_y - 4;
		rect.h = (12 * 36);
		
		SDL_FillRect (screen, &rect, blanco);
	}
	
	for (g = 0; g < 13; g++) {
		rect.y = pos_y + (g * 36) - 4;
		rect.h = 1;
		
		rect.x = pos_x - 4;
		rect.w = (6 * 38);
		
		SDL_FillRect (screen, &rect, blanco);
	}
	
	/* Draw all puffles on the map */
	for (g = 0; g < 12; g++) {
		for (h = 0; h < 6; h++) {
			if (map[g][h] == COLOR_NONE) {
				continue;
			}
			
			rect.x = pos_x + h * 38 - 8;
			rect.y = pos_y + (11 - g) * 36 - 8;
			
			/* If the puffle is falling, add the offset */
			if (falling_offsets[g][h] > 0) {
				rect.y = rect.y - falling_offsets[g][h];
			}
			
			image = Library::IMG_PUFFLE_BLUE_IDLE_1 + 6 * (map[g][h] - COLOR_1);
			
			/* Animate the puffle eyes */
			if (map_frames[g][h] == 96 || map_frames[g][h] == 101) {
				image += 1;
			} else if (map_frames[g][h] == 97 || map_frames[g][h] == 102) {
				image += 2;
			} else if (map_frames[g][h] >= 98 && map_frames[g][h] <= 100) {
				image += 3;
			}
			rect.w = library->images [image]->w;
			rect.h = library->images [image]->h;
			
			SDL_BlitSurface (library->images [image], NULL, screen, &rect);
			
			/* Draw the bubble */
			rect.x = pos_x + h * 38 - 8;
			rect.y = pos_y + (11 - g) * 36 - 8;
			if (falling_offsets[g][h] > 0) {
				rect.y = rect.y - falling_offsets[g][h];
			}
			
			rect.w = library->images [Library::IMG_BUBBLE]->w;
			rect.h = library->images [Library::IMG_BUBBLE]->h;
			
			SDL_BlitSurface (library->images [Library::IMG_BUBBLE], NULL, screen, &rect);
		}
	}
	
	/* Draw the falling piece */
	if (animating == MAP_ANIMATE_NONE) {
		f_p.draw (screen, pos_x, pos_y);
	}
	
	/* Draw the poped puffles */
	if (poped_end != poped_start) {
		//printf ("Drawing poped puffles: %i, %i\n", poped_start, poped_end);
		for (g = poped_start; g != poped_end; g = (g + 1) % MAX_POPED_PUFFLES) {
			if (poped[g].y > 660) continue; /* The puffle is out of the screen, skip */
			rect.x = poped[g].x;
			rect.y = poped[g].y;
			
			image = Library::IMG_PUFFLE_BLUE_FALL + 6 * (poped[g].color - COLOR_1);
			
			rect.w = library->images[image]->w;
			rect.h = library->images[image]->h;
			
			SDL_BlitSurface (library->images[image], NULL, screen, &rect);
			if (poped[g].frame == 0 || poped[g].frame == 1) {
				rect.x = poped[g].x - 6;
				rect.y = poped[g].y - 6;
				
				image = Library::IMG_BUBBLE_POP_1 + poped[g].frame;
				rect.w = library->images[image]->w;
				rect.h = library->images[image]->h;
				
				SDL_BlitSurface (library->images[image], NULL, screen, &rect);
			}
		}
	}
	
	/* Draw on-screen messages */
	msgs.draw (screen);
}

void Map::send_rotate_clock (void) {
	if (animating == MAP_ANIMATE_NONE) {
		f_p.rotate_clock (map);
	}
}

void Map::send_rotate_counter (void) {
	if (animating == MAP_ANIMATE_NONE) {
		f_p.rotate_counter (map);
	}
}

void Map::send_move_left (void) {
	if (animating == MAP_ANIMATE_NONE) {
		/* Validation is made inside the falling piece */
		f_p.move_left (map);
	}
}

void Map::send_move_right (void) {
	if (animating == MAP_ANIMATE_NONE) {
		/* Validation is made inside the falling piece */
		f_p.move_right (map);
	}
}

void Map::send_down (void) {
	if (animating == MAP_ANIMATE_NONE) {
		f_p.start_acel ();
	}
}

void Map::send_stop_down (void) {
	if (animating == MAP_ANIMATE_NONE) {
		f_p.stop_acel ();
	}
}

/* Main function to animate everything in the map */
void Map::animate (void) {
	int x1, x2, y1, y2;
	int color_1, color_2;
	int g, h;
	bool new_piece;
	
	/* Animate all puffles in map aka "Blinking eyes" */
	for (g = 0; g < 12; g++) {
		for (h = 0; h < 6; h++) {
			if (map[g][h] != COLOR_NONE) {
				map_frames[g][h]++;
				
				if (map_frames[g][h] == 103) {
					map_frames[g][h] = RANDOM(40);
				}
			}
		}
	}
	
	/* Animate the falling puffles */
	if (poped_end != poped_start) {
		for (g = poped_start; g != poped_end; g = (g + 1) % MAX_POPED_PUFFLES) {
			poped[g].x += poped[g].acel_x;
			poped[g].y += poped[g].acel_y;
			
			poped[g].acel_y += 1.0; /* Gravity */
			if (poped[g].acel_x > 0.0) {
				poped[g].acel_x -= 0.5;
			} else if (poped[g].acel_x < 0.0) {
				poped[g].acel_x += 0.5;
			}
			
			poped[g].frame++;
			
			if (poped[g].y > 660.0 && g == poped_start) {
				/* Delete this puffle */
				poped_start = (poped_start + 1) % MAX_POPED_PUFFLES;
			}
		}
	}
	
	if (animating == MAP_ANIMATE_GAMEOVER) {
		//printf ("Doing game overs: Pop start: %i, %i\n", poped_start, poped_end);
		for (g = 0; g < 12; g++) {
			for (h = 0; h < 6; h++) {
				if (map[g][h] != COLOR_NONE) {
					/* Make this puffle pop */
					if (poped_start == (poped_end + 1) % MAX_POPED_PUFFLES) {
						poped_start = (poped_start + 1) % MAX_POPED_PUFFLES;
					}
					poped[poped_end].x = pos_x + h * 38 - 8;
					poped[poped_end].y = pos_y + (11 - g) * 36 - 8;
					poped[poped_end].acel_y = RANDOM (4) * -1;
					poped[poped_end].acel_x = (3 + RANDOM (7)) * (RANDOM(2) == 0 ? 1 : -1);
					poped[poped_end].frame = 0;
					poped[poped_end].color = map[g][h];
					
					map[g][h] = COLOR_NONE;
					
					poped_end = (poped_end + 1) % MAX_POPED_PUFFLES;
					break; /* This will make the other for break; */
				}
			}
			
			if (h != 6) break;
		}
		
		if (g == 12) {
			/* All puffles have been poped, show a gameover text */
			animating = MAP_ANIMATE_SHOW_GAMEOVER;
		}
	}
	
	new_piece = false;
	if (animating == MAP_ANIMATE_NONE) {
		/* Check if the falling piece is at a boundary */
		if (f_p.has_falled ()) {
			/* Check now for collissions */
			f_p.get_xy (&x1, &y1, &x2, &y2);
			f_p.get_color (&color_1, &color_2);
		
			if (y1 == 0 || y2 == 0) {
				/* Bottom, stop the piece */
				/* Put the Piece on the map */
				map[y1][x1] = color_1;
				map[y2][x2] = color_2;
				
				map_frames[y1][x1] = RANDOM(40);
				map_frames[y2][x2] = RANDOM(40);
				
				new_piece = true;
				f_p.reset ();
			} else if (map[y1 - 1][x1] != COLOR_NONE) {
				/* Colission */
				map[y1][x1] = color_1;
				map_frames[y1][x1] = RANDOM(40);
				
				/* Check how much the other part will fall */
				for (g = y2; g > 0; g--) {
					if (map[g - 1][x2] != COLOR_NONE) {
						/* Stop here please */
						break;
					}
				}
				
				map[g][x2] = color_2;
				map_frames[y2][x2] = RANDOM(40);
				new_piece = true;
				
				f_p.reset ();
				if (y2 - g != 0) {
					falling_offsets[g][x2] = (y2 - g) * 36;
					animating = MAP_ANIMATE_FALLING;
					new_piece = false;
				}
			} else if (map[y2 - 1][x2] != COLOR_NONE) {
				/* Colission */
				f_p.get_color (&color_1, &color_2);
				
				map[y2][x2] = color_2;
				map_frames[y2][x2] = RANDOM(40);
				
				/* Check how much the other part will fall */
				for (g = y1; g > 0; g--) {
					if (map[g - 1][x1] != COLOR_NONE) {
						/* Stop here please */
						break;
					}
				}
				
				map[g][x1] = color_1;
				map_frames[y1][x1] = RANDOM(40);
				new_piece = true;
				f_p.reset ();
				if (y1 - g != 0) {
					falling_offsets[g][x1] = (y1 - g) * 36;
					animating = MAP_ANIMATE_FALLING;
					new_piece = false;
				}
			} else {
				f_p.fall ();
			}
		} else {
			f_p.fall ();
		}
	} else if (animating == MAP_ANIMATE_FALLING) {
		/* Decrement offsets to make pieces fall slowy */
		bool still = false;
		for (g = 0; g < 12; g++) {
			for (h = 0; h < 6; h++) {
				if (falling_offsets[g][h] != 0) {
					falling_offsets[g][h] -= 8;
					
					if (falling_offsets[g][h] > 0) still = true;
				}
			}
		}
		
		if (still == false) {
			/* Execute the island algorithm */
			new_piece = true;
			animating = MAP_ANIMATE_NONE;
		}
	}
	
	if (new_piece) {
		/* Check for pieces that may be together */
		check_islands ();
		
		/* If nothing has been erased, then send the next piece */
		if (animating == MAP_ANIMATE_NONE) {
			if (map[11][3] != COLOR_NONE) {
				/* Game over */
				animating = MAP_ANIMATE_GAMEOVER;
				//printf ("Game Over\n");
			} else {
				chain = 0;
				f_p.start_drop ();
			}
		}
	}
}

void Map::check_islands (void) {
	int visitados[15][6];
	int x, y, g, h;
	int i;
	bool was_chain = false;
	int max_combo = 0;
	int all_clear;
	
	/* Max island should never be more than 12, I hope */
	SDLPoint next[12];
	int color;
	int size;
	memset (visitados, 0, sizeof (visitados));
	
	for (y = 0; y < 12; y++) {
		for (x = 0; x < 6; x++) {
			if (visitados[y][x] == 1) continue;
			
			if (map[y][x] == COLOR_NONE) {
				visitados[y][x] = 1;
				continue;
			}
			
			size = 1;
			next[0].x = x;
			next[0].y = y;
			color = map[y][x];
			visitados[y][x] = 1;
			
			i = 0;
			while (i < size) {
				g = next[i].x;
				h = next[i].y;
				
				if (h > 0 && map[h - 1][g] == color && visitados[h - 1][g] == 0) {
					/* Add to the next list */
					next[size].x = g;
					next[size].y = h - 1;
					visitados[h - 1][g] = 1;
					size++;
				}
				if (h < 11 && map[h + 1][g] == color && visitados[h + 1][g] == 0) {
					next[size].x = g;
					next[size].y = h + 1;
					visitados[h + 1][g] = 1;
					size++;
				}
				if (g > 0 && map[h][g - 1] == color && visitados[h][g - 1] == 0) {
					next[size].x = g - 1;
					next[size].y = h;
					visitados[h][g - 1] = 1;
					size++;
				}
				if (g < 5 && map[h][g + 1] == color && visitados[h][g + 1] == 0) {
					next[size].x = g + 1;
					next[size].y = h;
					visitados[h][g + 1] = 1;
					size++;
				}
				i++;
			}
			
			if (size >= 4) {
				/* We have a 4 group */
				was_chain = true;
				
				animating = MAP_ANIMATE_FALLING;
				max_combo = max_combo + size;
				//printf ("We have a group of four:\n");
				for (i = 0; i < size; i++) {
					//printf ("Pos: %i, %i\n", next[i].x, next[i].y);
					g = next[i].x;
					h = next[i].y;
					/* Erase from the map */
					map[h][g] = COLOR_NONE;
					
					/* Add these puffles to the list of falled */
					/* If the list is full, erase the first one */
					if (poped_start == (poped_end + 1) % MAX_POPED_PUFFLES) {
						poped_start = (poped_start + 1) % MAX_POPED_PUFFLES;
					}
					poped[poped_end].x = pos_x + g * 38 - 8;
					poped[poped_end].y = pos_y + (11 - h) * 36 - 8;
					poped[poped_end].acel_y = RANDOM (4) * -1;
					poped[poped_end].acel_x = (3 + RANDOM (7)) * (RANDOM(2) == 0 ? 1 : -1);
					poped[poped_end].frame = 0;
					poped[poped_end].color = color;
					
					poped_end = (poped_end + 1) % MAX_POPED_PUFFLES;
				}
			}
		}
	}
	
	if (max_combo >= 5) {
		msgs.addCombo (max_combo);
	}
	
	if (was_chain) {
		chain++;
		if (chain >= 2) {
			/* Add Message chain */
			msgs.addChain (chain);
		}
	}
	
	all_clear = 0;
	/* Check if all clear bonus */
	for (x = 0; x < 6; x++) {
		for (y = 0; y < 12; y++) {
			if (map[y][x] == COLOR_NONE) {
				all_clear++;
			}
		}
	}
	
	if (was_chain && all_clear == 72) {
		/* Add "All clear" bonues */
		msgs.addAllClear ();
	}
	
	/* Now, compress the map */
	for (x = 0; x < 6; x++) {
		for (y = 0; y < 12; y++) {
			/* First, locate a empty spot */
			if (map[y][x] == COLOR_NONE) {
				g = x;
				
				for (i = y + 1; i < 13; i++) {
					if (map[i][x] != COLOR_NONE) {
						/* Move this piece */
						map[y][x] = map[i][x];
						map[i][x] = COLOR_NONE;
						
						falling_offsets[y][x] = (i - y) * 36;
						break;
					}
				}
				
				if (i == 12) break; /* We don't have more pieces up this column */
			}
		}
	}
}

