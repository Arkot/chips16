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
 * File:   types.h
 * Author: guillaume
 *
 * Created on 24 octobre 2011, 15:25
 */

#ifndef TYPES_H
#define	TYPES_H

typedef unsigned char Uint8;
typedef unsigned short int Uint16;
typedef char int8;
typedef short int16;

#define l 320
#define L 240
#define DIMPIXEL 3
#define WIDTH l*DIMPIXEL
#define HEIGHT L*DIMPIXEL

#define TAILLE_MEMOIRE 65536
#define ADRESSE_DEBUT 0
#define NBOPCODE 72
#define VITESSE_CPU 16666
#define FPS 16
//#define VITESSE_CPU 255
//#define FPS 160

using namespace std;

#endif	/* TYPES_H */