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
 * File:   main.cpp
 * Author: guillaume
 *
 * Created on 24 octobre 2011, 13:33
 */

#include <cstdlib>
#include <iostream>
#include <SDL/SDL.h>
#include "types.h"
#include "cpu.h"
#include "screen.h"

using namespace std;

cpu *c;

int main(int argc, char** argv) {
	Uint8 demarer = 0, cpt = 0, continuer = 1;
	
	c = new cpu();
	demarer = c->loadGame("ROMs/rom_test.c16");
//	demarer = c->loadGame("ROMs/Demos/Ball.c16");
//	demarer = c->loadGame("ROMs/Demos/Anim.c16");
//	demarer = c->loadGame("ROMs/Demos/GB16.c16");
//	demarer = c->loadGame("ROMs/Demos/ASCII.c16");
//	demarer = c->loadGame("ROMs/Demos/Triangle.c16");
//	demarer = c->loadGame("ROMs/Test roms/BC_TestRom.c16");
//	demarer = c->loadGame("ROMs/Test roms/flip_test.c16");
//	demarer = c->loadGame("ROMs/Games/Pong.c16");
	
	if(demarer==1) {
		while(continuer == 1) {
//			c->listen();
			
			for(cpt=0;cpt<VITESSE_CPU&&continuer==1;cpt++) {
				c->doAction(c->getOpcode());
				if(c->endOfRom()) continuer = 0;
			}
			
			
			c->ecran->updateScreen();
//			printf("\nUPDATE SCREEN\n");
//			c->decompter();
			
//			c->ecran->printScreen();
			
			SDL_Delay(FPS);
		}
	}
    return 0;
}
