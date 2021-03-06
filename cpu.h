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
 * File:   cpu.h
 * Author: Arkot
 *
 * Created on 24 octobre 2011, 14:49
 */

#ifndef CPU_H
#define	CPU_H

#include "types.h"
#include "screen.h"
#include "timer.h"
#include <iostream>
#include <cstdlib>

typedef struct {
	Uint32 masque[NBOPCODE];
	Uint32 id[NBOPCODE];
}JUMP;

class cpu {
    public:
        cpu();
        cpu(const cpu& orig);
        virtual ~cpu();
        
        void initialiserCpu();
		void initialiserJump();
		Uint32 getOpcode();
		Uint8 getAction(Uint32 opcode);
		Uint8 doAction(Uint32 opcode);
		Uint8 loadGame(char* file);
		void drawSprite(int16 b5,int16 b6,Uint16 b3);
		Uint8 endOfRom();
		Uint8 condition(Uint8 cond);
//		Uint8 listen();
//		Uint8 attendAppui(Uint8 b3);
		
		screen *ecran;
	private:
		void setFlag(Uint8 flag, Uint8 v);
		
        Uint8 ram[TAILLE_MEMOIRE];
        Uint16 pc, sp, stack;
        Uint16 V[16];
		Uint8 F, fliph, flipv, w, h, bg;
		Uint8 touche[16];
		Uint32 rom_size;
		JUMP jp;
		
		SDL_Event event;
		
		Uint32 nb_op;
		FILE *log,*dump;
		Timer timer;
};

#endif	/* CPU_H */

