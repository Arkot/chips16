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
 * File:   screen.cpp
 * Author: guillaume
 * 
 * Created on 24 octobre 2011, 15:27
 */

#include "screen.h"
#include <iostream>

screen::screen() {
	Uint8 i, j;
	
	initialiserEcran();
	initialiserPixels();
	updateScreen();
	
//	for(i=0;i<L;i++) {
//		for(j=0;j<l;j++) {
//			clearScreen();
//			ecran[j][i]->setColor(4);
//			updateScreen();
////			SDL_Delay(200);
//		}
//	}
}

screen::screen(const screen& orig) {
	initialiserEcran();
}

screen::~screen() {
	SDL_FreeSurface(carre[0]);
	SDL_FreeSurface(carre[1]);
    SDL_Quit();
}

void screen::clearScreen() {
	Uint16 i, j;
	for(i=0;i<l;i++) {
		for(j=0;j<L;j++) {
			ecran[i][j]->setColor(0);
		}
	}
}

void screen::initialiserPixels() {
	Uint16 i, j;
	for(i=0;i<l;i++) {
		for(j=0;j<L;j++) {
			ecran[i][j] = new pixel(DIMPIXEL, DIMPIXEL, i*DIMPIXEL, j*DIMPIXEL, 0);
//			ecran[i][j] = new pixel(DIMPIXEL, DIMPIXEL, i*DIMPIXEL, j*DIMPIXEL, (i+j)%16);
		}
	}
}

void screen::initialiserEcran() {
	Uint8 i;
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_WM_SetCaption("EmulChip16", NULL);
	
	surf = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); // Ouverture de la fenêtre
	if(surf == NULL) {
		cout << "Impossible de charger le mode vidéo : " << SDL_GetError() << endl;
		exit(0);
	}
	
	Uint8 colors[16][3] = {
		{0x00 ,0x00, 0x00}, // Noir (transparent)
		{0x00 ,0x00, 0x00}, // Noir
		{0x88 ,0x88, 0x88}, // Gris
		{0xBF ,0x39, 0x32}, // Rouge
		{0xDE ,0x7A, 0xAE}, // Rose
		{0x4C ,0x3D, 0x21}, // Brun
		{0x90 ,0x5F, 0x25}, // Marron
		{0xE4 ,0x94, 0x52}, // Orange
		{0xEA ,0xD9, 0x79}, // Jaune
		{0x53 ,0x7A, 0x3B}, // Vert
		{0xAB ,0xD5, 0x4A}, // Vert clair
		{0x25 ,0x2E, 0x38}, // Bleu foncé
		{0x00 ,0x47, 0x7F}, // Bleu
		{0x68 ,0xAB, 0xCC}, // Bleu clair
		{0xBC ,0xDE, 0xE4}, // Bleu ciel
		{0xFF ,0xFF, 0xFF}, // Blanc
	};
	
	for(i=0;i<16;i++) {
		carre[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, DIMPIXEL, DIMPIXEL, 32, 0, 0, 0, 0);
		if(carre[i]==NULL)
		{
		   fprintf(stderr,"Erreur lors du chargement de la surface %s",SDL_GetError());
		   exit(EXIT_FAILURE);
		}
		SDL_FillRect(carre[i], NULL, SDL_MapRGB(carre[i]->format, colors[i][0], colors[i][1], colors[i][2]));
	}
}

void screen::updateScreen() {
	for(int i=0;i<l;i++) {
		for(int j=0;j<L;j++) {
			drawPixel(ecran[i][j]);
		}
	}
	SDL_Flip(surf);
}

void screen::drawPixel(pixel *p) {
	SDL_BlitSurface(carre[p->getColor()], NULL, surf, p->getPosition());
}

void screen::drawPixel(Uint16 x, Uint16 y, Uint8 col) {
	ecran[x][y]->setPosition(x*DIMPIXEL,y*DIMPIXEL);
	ecran[x][y]->setColor(col);
	SDL_BlitSurface(carre[ecran[x][y]->getColor()], NULL, surf, ecran[x][y]->getPosition());
}

pixel* screen::getPixel(Uint16 x, Uint16 y) {
	return ecran[x][y];
}

void screen::printScreen() {
	Uint16 x,y;
	FILE* f = fopen("print.txt","w");
//	printf("\n\n********************************************************************\n");
	for(x=0;x<L;x++) {
		for(y=0;y<l;y++) {
			fprintf(f,"%x",ecran[y][x]->getColor());
//			printf("%d:%d\t",x,y);
		}
		fprintf(f,"\n");
	}
//	printf("********************************************************************\n\n");
	fclose(f);
}

void screen::setBackgroundColor(Uint8 bgc) {
	Uint16 i, j;
	for(i=0;i<l;i++) {
		for(j=0;j<L;j++) {
			ecran[i][j]->setColor(bgc);
		}
	}
	updateScreen();
}