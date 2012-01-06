/* 
 * Copyright (C) 2012 Arkot
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * File:   screen.h
 * Author: guillaume
 *
 * Created on 24 octobre 2011, 15:27
 */

#ifndef SCREEN_H
#define	SCREEN_H

#include "pixel.h"
#include <iostream>
#include <SDL/SDL.h>

class screen {
    public:
        screen();
        screen(const screen& orig);
        virtual ~screen();
		void initialiserEcran();
		void initialiserPixels();
		void drawPixel(pixel *p);
		void drawPixel(Uint8 x, Uint8 y, Uint8 col);
		pixel* getPixel(Uint8 x, Uint8 y);
		void setBackgroundColor(Uint8 bgc);
		void clearScreen();
		void updateScreen();
		void printScreen();
    private:
        pixel *ecran[l][L];
		SDL_Surface *surf, *carre[16];
};

#endif	/* SCREEN_H */

