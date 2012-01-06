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
 * File:   pixel.h
 * Author: guillaume
 *
 * Created on 24 octobre 2011, 15:17
 */

#ifndef PIXEL_H
#define	PIXEL_H

#include "types.h"
#include <SDL/SDL.h>

using namespace std;

class pixel {
	public:
		pixel();
//		pixel(const Uint8 width, const Uint8 height);
		pixel(const Uint8 width, const Uint8 height, const Uint16 xx, const Uint16 yy, const Uint8 col);
		pixel(const Uint8 width, const Uint8 height, const SDL_Rect position, const Uint8 col);
		pixel(const pixel& orig);
		virtual ~pixel();

		// Sets
		void setPosition(const Uint16 xx, const Uint16 yy);
		void setPosition(SDL_Rect *position);
		void setColor(const Uint8 col);

		// Gets
		SDL_Surface* getSurface();
		SDL_Rect* getPosition();
		Uint8 getColor();
	private:
		SDL_Rect *pos;
		SDL_Surface *surf;
		Uint8 color;
};

#endif	/* PIXEL_H */

