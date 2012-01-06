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
 * File:   pixel.cpp
 * Author: guillaume
 * 
 * Created on 24 octobre 2011, 15:17
 */

#include "pixel.h"
#include <iostream>

pixel::pixel() {
	pos = new SDL_Rect;
	pos->x = 0;
	pos->y = 0;
    color = 0;
}

pixel::pixel(const Uint8 width, const Uint8 height, const Uint16 xx, const Uint16 yy, const Uint8 col) {
	pos = new SDL_Rect;
	pos->x = xx;
	pos->y = yy;
    color = col;
}

pixel::pixel(const Uint8 width, const Uint8 height, const SDL_Rect position, const Uint8 col) {
	pos = new SDL_Rect;
	pos->x = position.x;
	pos->y = position.y;
    color = col;
}

pixel::pixel(const pixel& orig) {
	pos = new SDL_Rect;
	pos = orig.pos;
    color = orig.color;
}

pixel::~pixel() {
    
}

void pixel::setPosition(const Uint16 xx, const Uint16 yy) {
	pos = new SDL_Rect;
	pos->x = xx;
	pos->y = yy;
}

void pixel::setPosition(SDL_Rect *position) {
	pos = new SDL_Rect;
	pos = position;
}

void pixel::setColor(const Uint8 col) {
	color = col;
}

// Gets
SDL_Surface* pixel::getSurface() {
	return surf;
}
SDL_Rect* pixel::getPosition() {
	return pos;
}
Uint8 pixel::getColor() {
	return color;
}