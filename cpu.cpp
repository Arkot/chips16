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
 * File:   cpu.cpp
 * Author: guillaume
 * 
 * Created on 24 octobre 2011, 14:49
 */

/*
 *	VERIF :
 * 
 *	Flags add c OK - z OK - o OK - n OK
 *	Flags sub c OK - z OK - o OK - n OK
 *	Flags cmp c OK - z OK - o OK - n OK
 *	Flags and z OK - n OK
 *	Flags tst z OK - n OK
 *	Flags or  z OK - n OK
 *	Flags xor z OK - n OK
 *	Flags mul c OK - z OK - n OK
 *	Flags div c OK - z OK - n OK
 *	Flags shl z OK - n OK
 *	Flags shr z OK - n OK
 *
 */

#define DEBUG
#define TOLOG

#include "cpu.h"

Uint8 vblank;
Uint32 ttime(Uint32 i, void *p) {
	vblank=1;
//	printf("VBLANK = 1\n");
	return i;
}
cpu::cpu() {
	ecran = new screen();
    initialiserCpu();
	initialiserJump();
	srand(time(NULL));
	SDL_AddTimer(FPS,ttime,NULL);

#ifdef TOLOG
	log = fopen("logs.txt","w");
	dump = fopen("dump.txt","w");
#endif
}
cpu::cpu(const cpu& orig) {
	ecran = orig.ecran;
    initialiserCpu();
	initialiserJump();
	srand(time(NULL));
	SDL_AddTimer(FPS,ttime,NULL);
}
cpu::~cpu() {
#ifdef TOLOG
	fclose(log);
	fclose(dump);
#endif
}
void cpu::initialiserCpu() {
    for(int i=0;i<TAILLE_MEMOIRE;i++) {
        ram[i] = 0;
    }
    for(int i=0;i<16;i++) {
        V[i] = 0;
		touche[i] = 0;
    }
	vblank = 0;
	F = 0;
    pc = ADRESSE_DEBUT;
    sp = w = h = bg = fliph = flipv = 0;
	stack = 0xFDF0;
    
}
Uint32 cpu::getOpcode() {
	return (ram[pc]<<24)+(ram[pc+1]<<16)+(ram[pc+2]<<8)+ram[pc+3];
}
Uint8 cpu::getAction(Uint32 opcode) {
	Uint8 action;
	for(action=0;action<NBOPCODE;action++) {
		if((jp.masque[action]&opcode) == jp.id[action]) {
			break;
		}
	}
	return action;
}
Uint8 cpu::doAction(Uint32 opcode) {
	Uint8 continuer;
	Uint8 a=0,b8=0,b7=0,b6=0,b5=0,b4=0,b3=0,b2=0,b1=0,i;
		//int16 si1=0,si2=0,si3=0;
//	
////	timer.start();
//	
	a = getAction(opcode);
//	
////	timer.stop();
////	cout << "getAction    : " << timer.getElapsedTimeInMilliSec() << " ms.\n";
////	timer.start();
//	
	b8=(opcode&0xF0000000)>>28;
	b7=(opcode&0x0F000000)>>24;
	b6=(opcode&0x00F00000)>>20;
	b5=(opcode&0x000F0000)>>16;
	b4=(opcode&0x0000F000)>>12;
	b3=(opcode&0x00000F00)>>8;
	b2=(opcode&0x000000F0)>>4;
	b1=(opcode&0x0000000F);

#ifdef DEBUG
	printf("%6d | %x op:%x - a:%d | %x | %x:%x %x:%x %x:%x %x:%x | ",nb_op,pc,opcode,a,F,b8,b7,b6,b5,b4,b3,b2,b1);
#endif
	
#ifdef TOLOG
	fprintf(log,"%6d | %4x a:%2d | spr %2dx%2d | %x%x %x%x %x%x %x%x | c:%x z:%x o:%x n:%x | 0:%4x 1:%4x 2:%4x 3:%4x 4:%4x 5:%4x 6:%4x 7:%4x 8:%4x 9:%4x A:%4x B:%4x C:%4x D:%4x E:%4x F:%4x | ",nb_op,pc,a,w,h,b8,b7,b6,b5,b4,b3,b2,b1,(F&0x2)>>1,(F&0x4)>>2,(F&0x40)>>6,(F&0x80)>>7,V[0],V[1],V[2],V[3],V[4],V[5],V[6],V[7],V[8],V[9],V[10],V[11],V[12],V[13],V[14],V[15]);

	fprintf(dump,"\n\n---- %d - %x\n\n",nb_op,pc);
//	for(Uint16 j=0;j<0x2000;j++) {
	for(Uint32 j=0;j<TAILLE_MEMOIRE;j++) {
		fprintf(dump,"%x : %x\n",j,ram[j]);
	}
#endif
	
//	timer.stop();
//	cout << "avant switch : " << timer.getElapsedTimeInMilliSec() << " ms.\n";
//	timer.start();
	
	
	Uint8 rnd;
	switch(a) {
		case 0: // 00 00 00 00		NOP
#ifdef DEBUG
			printf("NOP");
#endif
#ifdef TOLOG
			fprintf(log,"NOP");
#endif
			break;
		case 1: // 01 00 00 00		CLS
			ecran->clearScreen();
#ifdef DEBUG
			printf("Clear screen");
#endif
#ifdef TOLOG
			fprintf(log,"Clear screen");
#endif
			break;
		case 2: // 02 00 00 00		VBLNK
			if(!vblank) pc -= 4;
#ifdef DEBUG
			printf("WAIT VBLANK");
#endif
#ifdef TOLOG
			fprintf(log,"WAIT VBLANK");
#endif
			break;
		case 3: // 03 00 0N 00		BGC N
			bg = b3;
			ecran->setBackgroundColor(bg);
#ifdef DEBUG
			printf("SET BG TO %x",b3);
#endif
#ifdef TOLOG
			fprintf(log,"SET BG TO %x",b3);
#endif
			break;
		case 4: // 04 00 LL HH		SPR HHLL
			w = (b4<<4)+b3;
			h = (b2<<4)+b1;
#ifdef DEBUG
			printf("SPR W:%d H:%d",(b4<<4)+b3,(b2<<4)+b1);
#endif
#ifdef TOLOG
			fprintf(log,"SPR W:%d H:%d",(b4<<4)+b3,(b2<<4)+b1);
#endif
			break;
		case 5: // 05 YX LL HH		DRW RX, RY, HHLL
			drawSprite(b5,b6,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#ifdef DEBUG
			printf("DRW (V[%x],V[%x]) coord (%d,%d) IMG adr %x",b5,b6,w,h,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"DRW (V[%x],V[%x]) coord (%d,%d) IMG adr %x",b5,b6,w,h,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 6: // 06 YX 0Z 00		DRW RX, RY, RZ
			drawSprite(b5,b6,V[b3]);
//			printf("DRW (%x,%x) IMG V[%x]\n",V[b5],V[b6],V[b3]);
#ifdef DEBUG
			printf("DRW (V[%x],V[%x]) coord (%d,%d) IMG V[%x]",b5,b6,w,h,b3);
#endif
#ifdef TOLOG
			fprintf(log,"DRW (V[%x],V[%x]) coord (%d,%d) IMG V[%x]",b5,b6,w,h,b3);
#endif
			break;
		case 7: // 07 0X LL HH		RND RX, HHLL
//			rnd = rand();
			V[b5] = rand()%(((b2<<12)+(b1<<8)+(b4<<4)+b3)+1);
//			V[b5] = rnd%(((b2<<12)+(b1<<8)+(b4<<4)+b3)+1);
//			printf("RND V[%x], MAX %x, %x | %d -- %d\n",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3,V[b5],rnd,rnd%(((b2<<12)+(b1<<8)+(b4<<4)+b3)+1));
#ifdef DEBUG
			printf("RND V[%x], MAX %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"RND V[%x], MAX %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 8: // 08 00 00 00		FLIP 0, 0
			fliph = flipv = 0;
#ifdef DEBUG
			printf("FLIP (0,0)");
#endif
#ifdef TOLOG
			fprintf(log,"FLIP (0,0)");
#endif
			break;
		case 9: // 08 00 00 01		FLIP 0, 1
			fliph = 0;
			flipv = 1;
#ifdef DEBUG
			printf("FLIP (0,1)");
#endif
#ifdef TOLOG
			fprintf(log,"FLIP (0,1)");
#endif
			break;
		case 10: // 08 00 00 02		FLIP 1, 0
			fliph = 1;
			flipv = 0;
#ifdef DEBUG
			printf("FLIP (1,0)");
#endif
#ifdef TOLOG
			fprintf(log,"FLIP (1,0)");
#endif
			break;
		case 11: // 08 00 00 03		FLIP 1, 1
			fliph = flipv = 1;
#ifdef DEBUG
			printf("FLIP (1,1)");
#endif
#ifdef TOLOG
			fprintf(log,"FLIP (1,1)");
#endif
			break;
		case 12: // 09 00 00 00		SND0
			
			break;
		case 13: // 0A 00 LL HH		SND1 HHLL
#ifdef DEBUG
			printf("SND1 for %d ms ** TODO",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"SND1 for %d ms ** TODO",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 14: // 0B 00 LL HH		SND2 HHLL
#ifdef DEBUG
			printf("SND2 for %d ms ** TODO",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"SND2 for %d ms ** TODO",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 15: // 0C 00 LL HH		SND3 HHLL
#ifdef DEBUG
			printf("SND3 for %d ms ** TODO",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"SND3 for %d ms ** TODO",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 16: // 0D 0X LL HH     SNP Rx, HHLL
			
			break;
		case 17: // 0E AD SR VT     SNG AD, VTSR
			
			break;
		case 18: // 10 00 LL HH		JMP HHLL
			pc = (b2<<12)+(b1<<8)+(b4<<4)+b3;
			pc -= 4;
#ifdef DEBUG
			printf("JMP %x",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"JMP %x",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 19: // 12 0x LL HH		Jx  HHLL
			if(condition(b5)) {
				pc = (b2<<12)+(b1<<8)+(b4<<4)+b3;
				pc -= 4;
			}
#ifdef DEBUG
			printf("JMP %x if cond[%x]",(b2<<12)+(b1<<8)+(b4<<4)+b3,b5);
#endif
#ifdef TOLOG
			fprintf(log,"JMP %x if cond[%x]",(b2<<12)+(b1<<8)+(b4<<4)+b3,b5);
#endif
			break;
		case 20: // 13 YX LL HH		JME RX, RY, HHLL
			if(V[b5]==V[b6]) {
				pc = (b2<<12)+(b1<<8)+(b4<<4)+b3;
				pc -= 4;
			}
#ifdef DEBUG
			printf("JMP %x if V[%x] = V[%x]",(b2<<12)+(b1<<8)+(b4<<4)+b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"JMP %x if V[%x] = V[%x]",(b2<<12)+(b1<<8)+(b4<<4)+b3,b5,b6);
#endif
			break;
		case 21: // 16 0X 00 00		JMP RX
			pc = V[b5];
			pc -= 4;
#ifdef DEBUG
			printf("JMP V[%x]",b5);
#endif
#ifdef TOLOG
			fprintf(log,"JMP V[%x]",b5);
#endif
			break;
		case 22: // 14 00 LL HH		CALL HHLL
			ram[stack+sp] = pc;
			sp += 2;
			pc = (b2<<12)+(b1<<8)+(b4<<4)+b3;
			pc -= 4;
#ifdef DEBUG
			printf("CALL %x",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"CALL %x",(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 23: // 15 00 00 00		RET
			sp -= 2;
			pc = ram[stack+sp];
//			pc -= 4;
#ifdef DEBUG
			printf("RET");
#endif
#ifdef TOLOG
			fprintf(log,"RET");
#endif
			break;
		case 24: // 17 0x LL HH     Cx   HHLL
			if(condition(b5)) {
				ram[stack+sp] = pc;
				sp += 2;
				pc = (b2<<12)+(b1<<8)+(b4<<4)+b3;
				pc -= 4;
			}
#ifdef DEBUG
			printf("CALL %x IF cond[%x]",(b2<<12)+(b1<<8)+(b4<<4)+b3,b5);
#endif
#ifdef TOLOG
			fprintf(log,"CALL %x IF cond[%x]",(b2<<12)+(b1<<8)+(b4<<4)+b3,b5);
#endif
			break;
		case 25: // 18 0X 00 00		CALL RX
			ram[stack+sp] = pc;
			sp += 2;
			pc = V[b5];
			pc -= 4;
#ifdef DEBUG
			printf("CALL V[%x]",b5);
#endif
#ifdef TOLOG
			fprintf(log,"CALL V[%x]",b5);
#endif
			break;
		case 26: // 20 0X LL HH		LDI RX, HHLL
			V[b5] = (b2<<12)+(b1<<8)+(b4<<4)+b3;
#ifdef DEBUG
			 printf("SET V[%x] = %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"SET V[%x] = %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 27: // 21 00 LL HH		LDI SP, HHLL
			sp = (b2<<12)+(b1<<8)+(b4<<4)+b3;
#ifdef DEBUG
			 printf("SET sp = %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"SET sp = %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 28: // 22 0X LL HH		LDM RX, HHLL
			V[b5] = (ram[(b2<<12)+(b1<<8)+(b4<<4)+b3]<<8)+ram[(b2<<12)+(b1<<8)+(b4<<4)+b3];
#ifdef DEBUG
			 printf("SET V[%x] = ram[%x]",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"SET V[%x] = ram[%x]",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 29: // 23 YX 00 00		LDM RX, RY	
			V[b5] = (ram[V[b6]]<<8)+ram[V[b6]+1];
#ifdef DEBUG
			 printf("SET V[%x] = ram[V[%x]]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"SET V[%x] = ram[V[%x]]",b5,b6);
#endif
			break;
		case 30: // 24 YX 00 00		MOV RX, RY	
			V[b5] = V[b6];
#ifdef DEBUG
			 printf("MOV V[%x] = V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"MOV V[%x] = V[%x]",b5,b6);
#endif
			break;
		case 31: // 30 0X LL HH		STM RX, HHLL
			ram[((b2<<12)+(b1<<8)+(b4<<4)+b3)+1] = V[b5]&0xff;
			ram[(b2<<12)+(b1<<8)+(b4<<4)+b3] = ((V[b5]&0xff00)>>8);
#ifdef DEBUG
			 printf("STM ram[%x] = V[%x] : ",(b2<<12)+(b1<<8)+(b4<<4)+b3,b5);
#endif
#ifdef TOLOG
			fprintf(log,"STM V[%x] : %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 32: // 31 YX 00 00		STM RX, RY
			ram[V[b6]+1] = V[b5]&0xff;
			ram[V[b6]] = ((V[b5]&0xff00)>>8);
#ifdef DEBUG
			 printf("STM ram[V[%x]] : V[%x]",b6,b5);
#endif
#ifdef TOLOG
			fprintf(log,"STM ram[V[%x]] : V[%x]",b6,b5);
#endif
			break;
		case 33: // 40 0X LL HH		ADDI RX, HHLL
			if((V[b5]+(b2<<12)+(b1<<8)+(b4<<4)+b3)>0xffff) setFlag(0x2, 1); else setFlag(0x2, 0);
			if(((Uint16)(V[b5]+(b2<<12)+(b1<<8)+(b4<<4)+b3))==0) setFlag(0x4, 1); else setFlag(0x4, 0);
			if(((V[b5]&0x8000)&&(((b2<<12)+(b1<<8)+(b4<<4)+b3)&0x8000)&&!((V[b5]+((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000))||(!(V[b5]&0x8000)&&!(((b2<<12)+(b1<<8)+(b4<<4)+b3)&0x8000)&&((V[b5]+((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000))) setFlag(0x40, 1); else setFlag(0x40, 0);
			if(((V[b5]+(b2<<12)+(b1<<8)+(b4<<4)+b3)&0x8000)) setFlag(0x80, 1); else setFlag(0x80, 0);
			V[b5] += (b2<<12)+(b1<<8)+(b4<<4)+b3;
#ifdef DEBUG
			 printf("ADD V[%x] : %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"ADD V[%x] : %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 34: // 41 YX 00 00		ADD RX, RY
			if((V[b5]+V[b6])>0xffff) setFlag(0x2, 1); else setFlag(0x2, 0);
			if(((Uint16)(V[b5]+V[b6])==0)) setFlag(0x4, 1); else setFlag(0x4, 0);
			if(((V[b5]&0x8000)&&(V[b6]&0x8000)&&!((V[b5]+V[b6])&0x8000))||(!(V[b5]&0x8000)&&!(V[b6]&0x8000)&&((V[b5]+V[b6])&0x8000))) setFlag(0x40, 1); else setFlag(0x40, 0);
			if(((V[b5]+V[b6])&0x8000)) setFlag(0x80, 1); else setFlag(0x80, 0);
			V[b5] += V[b6];
#ifdef DEBUG
			 printf("ADD V[%x] += V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"ADD V[%x] += V[%x]",b5,b6);
#endif
			break;
		case 35: // 42 YX 0Z 00		ADD RX, RY, RZ
			if((V[b5]+V[b6])>0xffff) setFlag(0x2, 1); else setFlag(0x2, 0);
			if(((Uint16)(V[b5]+V[b6])==0)) setFlag(0x4, 1); else setFlag(0x4, 0);
			if(((V[b5]&0x8000)&&(V[b6]&0x8000)&&!((V[b5]+V[b6])&0x8000))||(!(V[b5]&0x8000)&&!(V[b6]&0x8000)&&((V[b5]+V[b6])&0x8000))) setFlag(0x40, 1); else setFlag(0x40, 0);
			if(((V[b5]+V[b6])&0x8000)) setFlag(0x80, 1); else setFlag(0x80, 0);
			V[b3] = V[b5] + V[b6];
#ifdef DEBUG
			 printf("ADD V[%x] = V[%x] + V[%x]",b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"ADD V[%x] = V[%x] + V[%x]",b3,b5,b6);
#endif
			break;
		case 36: // 50 0X LL HH		SUBI RX, HHLL
			if((V[b5]<((b2<<12)+(b1<<8)+(b4<<4)+b3))) setFlag(0x2, 1); else setFlag(0x2, 0);
			if((V[b5]-((b2<<12)+(b1<<8)+(b4<<4)+b3))==0) setFlag(0x4, 1); else setFlag(0x4, 0);
			if((!(V[b5]&0x8000)&&(((b2<<12)+(b1<<8)+(b4<<4)+b3)&0x8000)&&((V[b5]+((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000))||((V[b5]&0x8000)&&!(((b2<<12)+(b1<<8)+(b4<<4)+b3)&0x8000)&&!((V[b5]+((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000))) setFlag(0x40, 1); else setFlag(0x40, 0);
			if((V[b5]-((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x80) setFlag(0x80, 1); else setFlag(0x80, 0);
			V[b5] -= ((b2<<12)+(b1<<8)+(b4<<4)+b3);
#ifdef DEBUG
			 printf("SUB V[%x] -= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"SUB V[%x] -= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 37: // 51 YX 00 00		SUB RX, RY
			if((V[b5]<V[b6])) setFlag(0x2, 1); else setFlag(0x2, 0);
			if((V[b5]-V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if((!(V[b5]&0x8000)&&(V[b6]&0x8000)&&((V[b5]-V[b6])&0x8000))||((V[b5]&0x8000)&&!(V[b6]&0x8000)&&!((V[b5]-V[b6])&0x8000))) setFlag(0x40, 1); else setFlag(0x40, 0);
			if((V[b5]-V[b6])&0x80) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5] -= V[b6];
#ifdef DEBUG
			 printf("SUB V[%x] -= V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"SUB V[%x] -= V[%x]",b5,b6);
#endif
			break;
		case 38: // 52 YX 0Z 00		SUB RX, RY, RZ
			if((V[b5]<V[b6])) setFlag(0x2, 1); else setFlag(0x2, 0);
			if((V[b5]-V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if((!(V[b5]&0x8000)&&(V[b6]&0x8000)&&((V[b5]-V[b6])&0x8000))||((V[b5]&0x8000)&&!(V[b6]&0x8000)&&!((V[b5]-V[b6])&0x8000))) setFlag(0x40, 1); else setFlag(0x40, 0);
			if((V[b5]-V[b6])&0x80==1) setFlag(0x80,1); else setFlag(0x80,0);
			V[b3] = V[b5] - V[b6];
#ifdef DEBUG
			 printf("SUB V[%x] = V[%x] - V[%x]",b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"SUB V[%x] = V[%x] - V[%x]",b3,b5,b6);
#endif
			break;
		case 39: // 53 0X LL HH		CMPI RX, HHLL
			if((V[b5]<((b2<<12)+(b1<<8)+(b4<<4)+b3))) setFlag(0x2, 1); else setFlag(0x2, 0);
			if((V[b5]-((b2<<12)+(b1<<8)+(b4<<4)+b3))==0) setFlag(0x4, 1); else setFlag(0x4, 0);
			if((!(V[b5]&0x8000)&&(((b2<<12)+(b1<<8)+(b4<<4)+b3)&0x8000)&&((V[b5]+((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000))||((V[b5]&0x8000)&&!(((b2<<12)+(b1<<8)+(b4<<4)+b3)&0x8000)&&!((V[b5]+((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000))) setFlag(0x40, 1); else setFlag(0x40, 0);
			if((V[b5]-((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x80) setFlag(0x80, 1); else setFlag(0x80, 0);
#ifdef DEBUG
			 printf("CMP V[%x] - %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"CMP V[%x] - %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 40: // 54 YX 00 00		CMP RX, RY
			if((V[b5]<V[b6])) setFlag(0x2, 1); else setFlag(0x2, 0);
			if((V[b5]-V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if((!(V[b5]&0x8000)&&(V[b6]&0x8000)&&((V[b5]-V[b6])&0x8000))||((V[b5]&0x8000)&&!(V[b6]&0x8000)&&!((V[b5]-V[b6])&0x8000))) setFlag(0x40, 1); else setFlag(0x40, 0);
			if((V[b5]-V[b6])&0x80) setFlag(0x80,1); else setFlag(0x80,0);
#ifdef DEBUG
			 printf("CMP V[%x] - V[%x]",b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"CMP V[%x] - V[%x]",b3,b5,b6);
#endif
			break;
		case 41: // 60 0X LL HH		ANDI RX, HHLL
			if((Uint16)(V[b5]&((b2<<12)+(b1<<8)+(b4<<4)+b3))==0) setFlag(0x4, 1); else setFlag(0x4, 0);
			if((Uint16)((V[b5]&((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000)) setFlag(0x80, 1); else setFlag(0x80, 0);
			V[b5] &= ((b2<<12)+(b1<<8)+(b4<<4)+b3);
#ifdef DEBUG
			 printf("AND V[%x] &= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"AND V[%x] &= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 42: // 61 YX 00 00		AND RX, RY
			if((Uint16)(V[b5]&V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if((Uint16)((V[b5]&V[b6])&0x8000)) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5] &= V[b6];
#ifdef DEBUG
			 printf("AND V[%x] &= V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"AND V[%x] &= V[%x]",b5,b6);
#endif
			break;
		case 43: // 62 YX 0Z 00		AND RX, RY, RZ
			if((Uint16)(V[b5]&V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if((Uint16)((V[b5]&V[b6])&0x80)) setFlag(0x8000,1); else setFlag(0x80,0);
			V[b3] = V[b5] & V[b6];
#ifdef DEBUG
			 printf("AND V[%x] = V[%x] & V[%x]",b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"AND V[%x] = V[%x] & V[%x]",b3,b5,b6);
#endif
			break;
		case 44: // 63 0X LL HH		TSTI RX, HHLL
			if((Uint16)(V[b5]&((b2<<12)+(b1<<8)+(b4<<4)+b3))==0) setFlag(0x4, 1); else setFlag(0x4, 0);
			if((Uint16)((V[b5]&((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000)) setFlag(0x80, 1); else setFlag(0x80, 0);
#ifdef DEBUG
			 printf("TST V[%x] & %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"TST V[%x] & %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 45: // 64 YX 00 00		TST RX, RY
			if((Uint16)(V[b5]&V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if((Uint16)((V[b5]&V[b6])&0x8000)) setFlag(0x80,1); else setFlag(0x80,0);
#ifdef DEBUG
			 printf("TST V[%x] & V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"TST V[%x] & V[%x]",b5,b6);
#endif
			break;
		case 46: // 70 0X LL HH		ORI RX, HHLL
			if((Uint16)(V[b5]|((b2<<12)+(b1<<8)+(b4<<4)+b3))==0) setFlag(0x4, 1); else setFlag(0x4, 0);
			if((Uint16)((V[b5]|((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000)) setFlag(0x80, 1); else setFlag(0x80, 0);
			V[b5] |= ((b2<<12)+(b1<<8)+(b4<<4)+b3);
#ifdef DEBUG
			 printf("OR V[%x] &= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"OR V[%x] &= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 47: // 71 YX 00 00		OR RX, RY
			if((Uint16)(V[b5]|V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]|V[b6]))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5] |= V[b6];
#ifdef DEBUG
			 printf("OR V[%x] |= V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"OR V[%x] |= V[%x]",b5,b6);
#endif
			break;
		case 48: // 72 YX 0Z 00		OR RX, RY, RZ
			if((Uint16)(V[b5]|V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if((Uint16)((V[b5]|V[b6])&0x8000)) setFlag(0x80,1); else setFlag(0x80,0);
			V[b3] = V[b5] | V[b6];
#ifdef DEBUG
			 printf("OR V[%x] = V[%x] | V[%x]",b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"OR V[%x] = V[%x] | V[%x]",b3,b5,b6);
#endif
			break;
		case 49: // 80 0X LL HH		XORI RX, HHLL
			if((V[b5]^((b2<<12)+(b1<<8)+(b4<<4)+b3))==0) setFlag(0x4, 1); else setFlag(0x4, 0);
			if(((V[b5]^((b2<<12)+(b1<<8)+(b4<<4)+b3))&0x8000)) setFlag(0x80, 1); else setFlag(0x80, 0);
			V[b5] ^= ((b2<<12)+(b1<<8)+(b4<<4)+b3);
#ifdef DEBUG
			 printf("XOR V[%x] ^= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"XOR V[%x] ^= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 50: // 81 YX 00 00		XOR RX, RY
			if((Uint16)(V[b5]^V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]^V[b6]))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5] ^= V[b6];
#ifdef DEBUG
			 printf("XOR V[%x] ^= V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"XOR V[%x] ^= V[%x]",b5,b6);
#endif
			break;
		case 51: // 82 YX 0Z 00		XOR RX, RY, RZ
			if((Uint16)(V[b5]^V[b6])==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]^V[b6]))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b3] = V[b5] ^ V[b6];
#ifdef DEBUG
			 printf("XOR V[%x] = V[%x] ^ V[%x]",b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"XOR V[%x] = V[%x] ^ V[%x]",b3,b5,b6);
#endif
			break;
		case 52: // 90 0X LL HH		MULI RX, HHLL
			if(((Uint32)(V[b5]*((b2<<12)+(b1<<8)+(b4<<4)+b3)))>0xffff) setFlag(0x2, 1); else setFlag(0x2, 0);
			if(((Uint16)(V[b5]*((b2<<12)+(b1<<8)+(b4<<4)+b3)))==0) setFlag(0x4, 1); else setFlag(0x4, 0);
			if(((Uint16)(V[b5]*((b2<<12)+(b1<<8)+(b4<<4)+b3)))&0x8000) setFlag(0x80, 1); else setFlag(0x80, 0);
			V[b5] *= ((b2<<12)+(b1<<8)+(b4<<4)+b3);
#ifdef DEBUG
			 printf("MUL V[%x] *= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"MUL V[%x] *= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 53: // 91 YX 00 00		MUL RX, RY
			if(((Uint32)(V[b5]*V[b6]))>0xffff) setFlag(0x2,1); else setFlag(0x2,0);
			if(((Uint16)(V[b5]*V[b6]))==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]*V[b6]))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5] *= V[b6];
#ifdef DEBUG
			 printf("MUL V[%x] *= V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"MUL V[%x] *= V[%x]",b5,b6);
#endif
			break;
		case 54: // 92 YX 0Z 00		MUL RX, RY, RZ
			if(((Uint32)(V[b5]*V[b6]))>0xffff) setFlag(0x2,1); else setFlag(0x2,0);
			if(((Uint16)(V[b5]*V[b6]))==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]*V[b6]))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b3] = V[b5] * V[b6];
#ifdef DEBUG
			 printf("MUL V[%x] = V[%x] * V[%x]",b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"MUL V[%x] = V[%x] * V[%x]",b3,b5,b6);
#endif
			break;
		case 55: // A0 0X LL HH		DIVI RX, HHLL
			if(V[b5]%((b2<<12)+(b1<<8)+(b4<<4)+b3)) setFlag(0x2,1); else setFlag(0x2,0);
			if(((Uint16)(V[b5]/((b2<<12)+(b1<<8)+(b4<<4)+b3)))==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]/((b2<<12)+(b1<<8)+(b4<<4)+b3)))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5] /= (((b2<<12)+(b1<<8)+(b4<<4)+b3));
#ifdef DEBUG
			 printf("DIV V[%x] /= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
#ifdef TOLOG
			fprintf(log,"DIV V[%x] /= %x",b5,(b2<<12)+(b1<<8)+(b4<<4)+b3);
#endif
			break;
		case 56: // A1 YX 00 00		DIV RX, RY
			if(V[b5]%V[b6]) setFlag(0x2,1); else setFlag(0x2,0);
			if(((Uint16)(V[b5]/V[b6]))==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]/V[b6]))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5] /= V[b6];
#ifdef DEBUG
			 printf("DIV V[%x] /= V[%x]",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"DIV V[%x] /= V[%x]",b5,b6);
#endif
			break;
		case 57: // A2 YX 0Z 00		DIV RX, RY, RZ
			if(V[b5]%V[b6]) setFlag(0x2,1); else setFlag(0x2,0);
			if(((Uint16)(V[b5]/V[b6]))==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]/V[b6]))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b3] = V[b5] / V[b6];
#ifdef DEBUG
			 printf("DIV V[%x] = V[%x] / V[%x]",b3,b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"DIV V[%x] = V[%x] / V[%x]",b3,b5,b6);
#endif
			break;
		case 58: // B0 0X 0N 00		SHL RX, N
			if((Uint16)(V[b5]<<b3)==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]<<b3))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5]<<=b3;
#ifdef DEBUG
			 printf("SHL V[%x] <<= %x",b5,b3);
#endif
#ifdef TOLOG
			fprintf(log,"SHL V[%x] <<= %x",b5,b3);
#endif
			break;
		case 59: // B1 0X 0N 00		SHR RX, N
			if((Uint16)(V[b5]>>b3)==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]>>b3))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5]>>=b3;
#ifdef DEBUG
			 printf("SHR V[%x] >>= %x",b5,b3);
#endif
#ifdef TOLOG
			fprintf(log,"SHR V[%x] >>= %x",b5,b3);
#endif
			break;
		case 60: // B2 0X 0N 00		SAR RX, N
			if((Uint16)(V[b5]>>b3)==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]>>b3))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5]>>=b3;
#ifdef DEBUG
			 printf("SHR V[%x] >>= %x",b5,b3);
#endif
#ifdef TOLOG
			fprintf(log,"SHR V[%x] >>= %x",b5,b3);
#endif
			break;
		case 61: // B3 YX 00 00		SHL RX, RY
			printf("\n%x << %x = %x\n",V[b5],(V[b6]&0xf),V[b5]<<b3);
			if((Uint16)(V[b5]<<(V[b6]&0xf))==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]<<(V[b6]&0xf)))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5]<<=(V[b6]&0xf);
#ifdef DEBUG
			 printf("SHL V[%x] >>= V[%x]&0xf",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"SHL V[%x] >>= V[%x]&0xf",b5,b6);
#endif
			break;
		case 62: // B4 YX 00 00		SHR RX, RY	
			if((Uint16)(V[b5]>>(V[b6]&0xf))==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]>>(V[b6]&0xf)))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5]>>=(V[b6]&0xf);
#ifdef DEBUG
			 printf("SHR V[%x] >>= V[%x]&0xf",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"SHR V[%x] >>= V[%x]&0xf",b5,b6);
#endif
			break;
		case 63: // B5 YX 00 00		SAR RX, RY	
			if((Uint16)(V[b5]>>(V[b6]&0xf))==0) setFlag(0x4,1); else setFlag(0x4,0);
			if(((Uint16)(V[b5]>>(V[b6]&0xf)))&0x8000) setFlag(0x80,1); else setFlag(0x80,0);
			V[b5]>>=(V[b6]&0xf);
#ifdef DEBUG
			 printf("SHR V[%x] >>= V[%x]&0xf",b5,b6);
#endif
#ifdef TOLOG
			fprintf(log,"SHR V[%x] >>= V[%x]&0xf",b5,b6);
#endif
			break;
		case 64: // C0 0X 00 00		PUSH RX
			ram[stack+sp] = (V[b5]&0xff00)>>8;
			ram[stack+sp+1] = V[b5]&0xff;
			sp += 2;
#ifdef DEBUG
			printf("PUSH RX");
#endif
#ifdef TOLOG
			fprintf(log,"PUSH RX");
#endif
			break;
		case 65: // C1 0X 00 00		POP  RX
			sp -= 2;
			V[b5] = (ram[stack+sp]<<8) + ram[stack+sp+1];
#ifdef DEBUG
			printf("POP RX");
#endif
#ifdef TOLOG
			fprintf(log,"POP RX");
#endif
			break;
		case 66: // C2 00 00 00		PUSHALL
			for(i=0;i<0x10;i++) {
				ram[stack+sp+(i*2)] = (V[i]&0xff00)>>8;
				ram[stack+sp+(i*2)+1] = V[i]&0xff;
			}
			sp += 32;
#ifdef DEBUG
			printf("PUSHALL");
#endif
#ifdef TOLOG
			fprintf(log,"PUSHALL");
#endif
			break;
		case 67: // C3 00 00 00		POPALL
			sp -= 32;
			for(i=0;i<0x10;i++) {
				V[i] = (ram[stack+sp+(i*2)]<<8) + ram[stack+sp+(i*2)+1];
			}
#ifdef DEBUG
			printf("POPALL");
#endif
#ifdef TOLOG
			fprintf(log,"POPALL");
#endif
			break;
		case 68: // C4 00 00 00		PUSHF
			ram[stack+sp] = F;
			sp += 2;
#ifdef DEBUG
			printf("PUSHF");
#endif
#ifdef TOLOG
			fprintf(log,"PUSHF");
#endif
			break;
		case 69: // C5 00 00 00		POPF
			sp -= 2;
			F = ram[stack+sp];
#ifdef DEBUG
			printf("POPF");
#endif
#ifdef TOLOG
			fprintf(log,"POPF");
#endif
			break;
		case 70: // D0 00 LL HH     PAL HHLL
#ifdef DEBUG
			printf("PAL HHLL");
#endif
#ifdef TOLOG
			fprintf(log,"PAL HHLL");
#endif
			break;
		case 71: // D1 0x 00 00     PAL Rx
#ifdef DEBUG
			printf("PAL Rx");
#endif
#ifdef TOLOG
			fprintf(log,"PAL Rx");
#endif
			break;
		default:
#ifdef DEBUG
			printf("\n\n*************************************************************************\n"
				"Unknow instruction\n*************************************************************************\n\n");
#endif
			break;
	}
	
//	timer.stop();
//	cout << "apres switch : " << timer.getElapsedTimeInMilliSec() << " ms.\n";
//	timer.start();
	
#ifdef DEBUG
	printf("\n");
#endif
#ifdef TOLOG
	fprintf(log,"\n");
	nb_op++;
#endif
	vblank=0;
	pc+=4;
	return continuer;
}
void cpu::initialiserJump() {
	jp.masque[0]=0xFFFFFFFF; jp.id[0]=0x00000000; // 00 00 00 00
	jp.masque[1]=0xFFFFFFFF; jp.id[1]=0x01000000; // 01 00 00 00
	jp.masque[2]=0xFFFFFFFF; jp.id[2]=0x02000000; // 02 00 00 00
	jp.masque[3]=0xFFFFF0FF; jp.id[3]=0x03000000; // 03 00 0N 00
	jp.masque[4]=0xFFFF0000; jp.id[4]=0x04000000; // 04 00 LL HH
	jp.masque[5]=0xFF000000; jp.id[5]=0x05000000; // 05 XY LL HH
	jp.masque[6]=0xFF00F0FF; jp.id[6]=0x06000000; // 06 XY 0Z 00
	jp.masque[7]=0xFFF00000; jp.id[7]=0x07000000; // 07 0X LL HH
	jp.masque[8]=0xFFFFFFFF; jp.id[8]=0x08000000; // 08 00 00 00
	jp.masque[9]=0xFFFFFFFF; jp.id[9]=0x08000001; // 08 00 00 01
	jp.masque[10]=0xFFFFFFFF; jp.id[10]=0x08000002; // 08 00 00 02
	jp.masque[11]=0xFFFFFFFF; jp.id[11]=0x08000003; // 08 00 00 03
	jp.masque[12]=0xFFFFFFFF; jp.id[12]=0x09000000; // 09 00 00 00
	jp.masque[13]=0xFFFF0000; jp.id[13]=0x0A000000; // 0A 00 LL HH
	jp.masque[14]=0xFFFF0000; jp.id[14]=0x0B000000; // 0B 00 LL HH
	jp.masque[15]=0xFFFF0000; jp.id[15]=0x0C000000; // 0C 00 LL HH
	jp.masque[16]=0xFFF00000; jp.id[16]=0x0D000000; // 0D 0X LL HH
	jp.masque[17]=0xFF000000; jp.id[17]=0x0E000000; // 0E AD SR VT
	jp.masque[18]=0xFFFF0000; jp.id[18]=0x10000000; // 10 00 LL HH
	jp.masque[19]=0xFFF00000; jp.id[19]=0x12000000; // 12 0X LL HH
	jp.masque[20]=0xFF000000; jp.id[20]=0x13000000; // 13 YX LL HH
	jp.masque[21]=0xFFF0FFFF; jp.id[21]=0x16000000; // 16 0X 00 00
	jp.masque[22]=0xFFFF0000; jp.id[22]=0x14000000; // 14 00 LL HH
	jp.masque[23]=0xFFFFFFFF; jp.id[23]=0x15000000; // 15 00 00 00
	jp.masque[24]=0xFFF00000; jp.id[24]=0x17000000; // 17 0X LL HH
	jp.masque[25]=0xFFF0FFFF; jp.id[25]=0x18000000; // 18 0X 00 00
	jp.masque[26]=0xFFF00000; jp.id[26]=0x20000000; // 20 0X LL HH
	jp.masque[27]=0xFFFF0000; jp.id[27]=0x21000000; // 21 00 LL HH
	jp.masque[28]=0xFFF00000; jp.id[28]=0x22000000; // 22 0X LL HH
	jp.masque[29]=0xFF00FFFF; jp.id[29]=0x23000000; // 23 XY 00 00
	jp.masque[30]=0xFF00FFFF; jp.id[30]=0x24000000; // 24 XY 00 00
	jp.masque[31]=0xFFF00000; jp.id[31]=0x30000000; // 30 0X LL HH
	jp.masque[32]=0xFF00FFFF; jp.id[32]=0x31000000; // 31 YX 00 00
	jp.masque[33]=0xFFF00000; jp.id[33]=0x40000000; // 40 0X LL HH
	jp.masque[34]=0xFF00FFFF; jp.id[34]=0x41000000; // 41 YX 00 00
	jp.masque[35]=0xFF00F0FF; jp.id[35]=0x42000000; // 42 YX 0Z 00
	jp.masque[36]=0xFFF00000; jp.id[36]=0x50000000; // 50 0X LL HH
	jp.masque[37]=0xFF00FFFF; jp.id[37]=0x51000000; // 51 YX 00 00
	jp.masque[38]=0xFF00F0FF; jp.id[38]=0x52000000; // 52 YX 0Z 00
	jp.masque[39]=0xFFF00000; jp.id[39]=0x53000000; // 53 0X LL HH
	jp.masque[40]=0xFF00FFFF; jp.id[40]=0x54000000; // 54 YX 00 00
	jp.masque[41]=0xFFF00000; jp.id[41]=0x60000000; // 60 0X LL HH
	jp.masque[42]=0xFF00FFFF; jp.id[42]=0x61000000; // 61 YX 00 00
	jp.masque[43]=0xFF00F0FF; jp.id[43]=0x62000000; // 62 YX 0Z 00
	jp.masque[44]=0xFFF00000; jp.id[44]=0x63000000; // 63 0X LL HH
	jp.masque[45]=0xFF00FFFF; jp.id[45]=0x64000000; // 64 YX 00 00
	jp.masque[46]=0xFFF00000; jp.id[46]=0x70000000; // 70 0X LL HH
	jp.masque[47]=0xFF00FFFF; jp.id[47]=0x71000000; // 71 YX 00 00
	jp.masque[48]=0xFF00F0FF; jp.id[48]=0x72000000; // 72 YX 0Z 00
	jp.masque[49]=0xFFF00000; jp.id[49]=0x80000000; // 80 0X LL HH
	jp.masque[50]=0xFF00FFFF; jp.id[50]=0x81000000; // 81 YX 00 00
	jp.masque[51]=0xFF00F0FF; jp.id[51]=0x82000000; // 82 YX 0Z 00
	jp.masque[52]=0xFFF00000; jp.id[52]=0x90000000; // 90 0X LL HH
	jp.masque[53]=0xFF00FFFF; jp.id[53]=0x91000000; // 91 YX 00 00
	jp.masque[54]=0xFF00F0FF; jp.id[54]=0x92000000; // 92 YX 0Z 00
	jp.masque[55]=0xFFF00000; jp.id[55]=0xA0000000; // A0 0X LL HH
	jp.masque[56]=0xFF00FFFF; jp.id[56]=0xA1000000; // A1 YX 00 00
	jp.masque[57]=0xFF00F0FF; jp.id[57]=0xA2000000; // A2 YX 0Z 00
	jp.masque[58]=0xFFF0F0FF; jp.id[58]=0xB0000000; // B0 0X 0N 00
	jp.masque[59]=0xFFF0F0FF; jp.id[59]=0xB1000000; // B1 0X 0N 00
	jp.masque[60]=0xFFF0F0FF; jp.id[60]=0xB2000000; // B2 0X 0N 00
	jp.masque[61]=0xFF00FFFF; jp.id[61]=0xB3000000; // B3 YX 00 00
	jp.masque[62]=0xFF00FFFF; jp.id[62]=0xB4000000; // B4 YX 00 00
	jp.masque[63]=0xFF00FFFF; jp.id[63]=0xB5000000; // B5 YX 00 00
	jp.masque[64]=0xFFF0FFFF; jp.id[64]=0xC0000000; // C0 0X 00 00
	jp.masque[65]=0xFFF0FFFF; jp.id[65]=0xC1000000; // C1 0X 00 00
	jp.masque[66]=0xFFFFFFFF; jp.id[66]=0xC2000000; // C2 00 00 00
	jp.masque[67]=0xFFFFFFFF; jp.id[67]=0xC3000000; // C3 00 00 00
	jp.masque[68]=0xFFFFFFFF; jp.id[68]=0xC4000000; // C4 00 00 00
	jp.masque[69]=0xFFFFFFFF; jp.id[69]=0xC5000000; // C5 00 00 00
	jp.masque[70]=0xFFFF0000; jp.id[70]=0xD0000000; // D0 00 LL HH
	jp.masque[71]=0xFFF0FFFF; jp.id[71]=0xD1000000; // D1 0X 00 00
}
void cpu::drawSprite(int16 b5,int16 b6,Uint16 adr) {
	Uint8 c=0,carry=0;
	Uint16 i,j;
	int16 x,y;
	Uint32 a=0;
	
//	for(j=0;j<TAILLE_MEMOIRE;j++) {
//		printf("%x : %x\n",j,ram[j]);
//	}
	
#ifdef DEBUG
	printf("\n");
#endif
	
//	timer.start();
	
	for(j=0;j<w;j++) {
		y = (((int16)V[b6])+j);
		for(i=0;i<h;i++) {
			x = (((int16)V[b5])+i);
			if(x<0 || y<0) continue;
//			if((a%2)==0) c=((ram[adr+(a/2)]&0xf0)>>4); else c=(ram[adr+(a/2)]&0x0f);
			if(a^=1) c=((ram[adr+(a/2)]&0xf0)>>4); else c=(ram[adr+(a/2)]&0x0f);
			if(c!=0 && ecran->getPixel(x,y)->getColor()!=0) carry=1;
			if(c==0) c=bg;
//			timer.start();
			ecran->drawPixel(x,y,c);
//			timer.stop();
//			cout << timer.getElapsedTimeInMilliSec() << " ms.\n";
#ifdef DEBUG
			printf("(%d,%d) : %x\n",x,y,c);
#endif
			a++;
		}
	}
	if(carry) setFlag(0x2,1); else setFlag(0x2,0);
	
//	timer.stop();
//	cout << timer.getElapsedTimeInMilliSec() << " ms.\n";
//	ecran->printScreen();
}
Uint8 cpu::loadGame(char* file) {
	FILE *jeu=NULL;
	Uint32 i;
	jeu=fopen(file, "rb");
	if(jeu!=NULL) {
		rom_size = fread(&ram[ADRESSE_DEBUT], sizeof(Uint8), (TAILLE_MEMOIRE-ADRESSE_DEBUT), jeu);
		fclose(jeu);
		return 1;
	}
	else {
		cout << "Erreur de chargement du jeu" << endl;
		return 0;
	}
}
Uint8 cpu::endOfRom() {
	if(pc>rom_size) return 1; else return 0;
}
//Uint8 cpu::listen() {
//	Uint8 continuer=1;
//	while( SDL_PollEvent(&event))
//	{
//		switch(event.type)
//		{
//			case SDL_QUIT: continuer = 0;break;
//			case SDL_KEYDOWN:
//				switch(event.key.keysym.sym)
//				{
//					case SDLK_KP0: touche[0x0]=1;break;
//					case SDLK_KP7: touche[0x1]=1;break;
//					case SDLK_KP8: touche[0x2]=1;break;
//					case SDLK_KP9: touche[0x3]=1;break;
//					case SDLK_KP4: touche[0x4]=1;break;
//					case SDLK_KP5: touche[0x5]=1;break;
//					case SDLK_KP6: touche[0x6]=1;break;
//					case SDLK_KP1: touche[0x7]=1;break;
//					case SDLK_KP2: touche[0x8]=1;break;
//					case SDLK_KP3: touche[0x9]=1;break;
//					case SDLK_RIGHT: touche[0xA]=1;break;
//					case SDLK_KP_PERIOD:touche[0xB]=1;break;
//					case SDLK_KP_MULTIPLY:touche[0xC]=1;break;
//					case SDLK_KP_MINUS:touche[0xD]=1;break;
//					case SDLK_KP_PLUS:touche[0xE]=1;break;
//					case SDLK_KP_ENTER:touche[0xF]=1;break;
//					case SDLK_p: pause();break;
////					case SDLK_r: reset();break;
//
//					default:{ break;}
//				} ;break;
//			case SDL_KEYUP:
//				switch(event.key.keysym.sym)
//				{
//					case SDLK_KP0: touche[0x0]=0;break;
//					case SDLK_KP7: touche[0x1]=0;break;
//					case SDLK_KP8: touche[0x2]=0;break;
//					case SDLK_KP9: touche[0x3]=0;break;
//					case SDLK_KP4: touche[0x4]=0;break;
//					case SDLK_KP5: touche[0x5]=0;break;
//					case SDLK_KP6: touche[0x6]=0;break;
//					case SDLK_KP1: touche[0x7]=0;break;
//					case SDLK_KP2: touche[0x8]=0;break;
//					case SDLK_KP3: touche[0x9]=0;break;
//					case SDLK_RIGHT: touche[0xA]=0;break;
//					case SDLK_KP_PERIOD:touche[0xB]=0;break;
//					case SDLK_KP_MULTIPLY:touche[0xC]=0;break;
//					case SDLK_KP_MINUS:touche[0xD]=0;break;
//					case SDLK_KP_PLUS:touche[0xE]=0;break;
//					case SDLK_KP_ENTER:touche[0xF]=0;break;
//
//					default: break;
//				} ;break;
//			default: break;
//		}
//	}
//	return continuer;
//}
//Uint8 cpu::attendAppui(Uint8 b3) {
//	Uint8 attend=1,continuer=1;
//	while(attend)
//	{
//		SDL_WaitEvent(&event);
//		switch(event.type)
//		{
//			case SDL_QUIT: continuer=0;attend=0; break;
//			case SDL_KEYDOWN:
//				switch(event.key.keysym.sym)
//				{
//					case SDLK_KP0:			V[b3]=0x0;	touche[0x0]=1;	attend=0;break;
//					case SDLK_KP7:			V[b3]=0x1;	touche[0x1]=1;	attend=0;break;
//					case SDLK_KP8:			V[b3]=0x2;	touche[0x2]=1;	attend=0;break;
//					case SDLK_KP9:			V[b3]=0x3;	touche[0x3]=1;	attend=0;break;
//					case SDLK_KP4:			V[b3]=0x4;	touche[0x4]=1;	attend=0;break;
//					case SDLK_KP5:			V[b3]=0x5;	touche[0x5]=1;	attend=0;break;
//					case SDLK_KP6:			V[b3]=0x6;	touche[0x6]=1;	attend=0;break;
//					case SDLK_KP1:			V[b3]=0x7;	touche[0x7]=1;	attend=0;break;
//					case SDLK_KP2:			V[b3]=0x8;	touche[0x8]=1;	attend=0;break;
//					case SDLK_KP3:			V[b3]=0x9;	touche[0x9]=1;	attend=0;break;
//					case SDLK_RIGHT:        V[b3]=0xA;	touche[0xA]=1;	attend=0;break;
//					case SDLK_KP_PERIOD:    V[b3]=0xB;	touche[0xB]=1;	attend=0;break;
//					case SDLK_KP_MULTIPLY:  V[b3]=0xC;	touche[0xC]=1;	attend=0;break;
//					case SDLK_KP_MINUS:     V[b3]=0xD;	touche[0xD]=1;	attend=0;break;
//					case SDLK_KP_PLUS:      V[b3]=0xE;	touche[0xE]=1;	attend=0;break;
//					case SDLK_KP_ENTER:     V[b3]=0xF;	touche[0xF]=1;	attend=0;break;
//					default:{ break;}
//				} break;
//			default: break;
//		}
//	}
//	 return continuer;
//}
void cpu::setFlag(Uint8 flag, Uint8 v) {
//	printf("\n%x : %x\nf:%x\n",flag,v,F);
	if((v&&!(F&flag)) || (!v&&(F&flag))) F^=flag;
//	printf("f:%x\n",F);
}
Uint8 cpu::condition(Uint8 cond) {
	/* 
	 * Test :
	 *	 0 : OK
	 *	 1 : OK
	 *	 2 : OK
	 *	 3 : OK
	 *	 4 : OK
	 *	 5 : OK
	 *	 6 : OK
	 *	 7 : OK
	 *	 8 : OK
	 *	 9 : OK
	 *	10 : OK
	 *	11 : OK
	 *	12 : OK
	 *	13 : OK
	 *	14 : OK
	 *	15 : OK
	 * 
	 **/
	
	switch(cond) {
		case   0: if(F&0x4) return 1; else return 0; break;
		case   1: if(F&0x4) return 0; else return 1; break;
		case   2: if(F&0x80) return 1; else return 0; break;
		case   3: if(F&0x80) return 0; else return 1; break;
		case   4: if(!(F&0x80) && !(F&0x4)) return 1; else return 0; break;
		case   5: if(F&0x40) return 1; else return 0; break;
		case   6: if(F&0x40) return 0; else return 1; break;
		case   7: if(!(F&0x2) && !(F&0x4)) return 1; else return 0; break;
		case   8: if(F&0x2) return 0; else return 1;break;
		case   9: if(F&0x2) return 1; else return 0;break;
		case 0xa: if(F&0x2 || F&0x4) return 1; else return 0; break;
		case 0xb: if(((F&0x40)>>6)==((F&0x80)>>7) && !(F&0x4)) return 1; else return 0; break;
		case 0xc: if(((F&0x40)>>6)==((F&0x80)>>7)) return 1; else return 0; break;
		case 0xd: if(((F&0x40)>>6)!=((F&0x80)>>7)) return 1; else return 0; break;
		case 0xe: if(((F&0x40)>>6)!=((F&0x80)>>7) || (F&0x4)) return 1; else return 0; break;
		case 0xf: break;
	}
}