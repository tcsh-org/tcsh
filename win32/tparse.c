/*-
 * Copyright (c) 1980, 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * tparse.c: hacks to parse termcap sequences.
 *
 */
#include <stdio.h>
#include <string.h>
#include <ntport.h>

#define AL 'a'
#define BL 'b'
#define BT 'c'
#define CD 'd'
#define CE 'e'
#define CL 'f'
#define CR '\015'
#define DC 'g'
#define DL 'h'
#define DO 'A'
#define HO 'B'
#define KB '\008'
#define KD 'D'
#define KH 'E'
#define KF 'F'
#define KR 'G'
#define KU 'H'
#define LE 'I'
#define ND 'J'
#define SE 'K'
#define SF '\012'
#define SO 'M'
#define TA '\011'
#define UE 'O'
#define UP 'P'
#define US 'Q'
#define UPPER_LE 'R'
#define UPPER_RI 'S'
#define UPPER_UP 'T'
#define UPPER_DO 'U'

static char mult_buf[6];
static int  curr_pos;
static int state;
#define PARSING_MULT 1

int move_cursor(char,int) ;
extern void NT_MoveToLineOrChar(int,int);
extern void nt_move_next_tab(void);
extern int nt_ClearEOL( void) ;
extern void NT_ClearEOD( void) ;
extern void NT_ClearScreen(void) ;
int tc_fd_putc(char c, int fd) {

	int rc=0;
	char *tmp;

	if( (state == PARSING_MULT) &&( c != 'Z') ){
		mult_buf[curr_pos++]=c;
		return 0;
	}
	switch (c) {
		case CD:
			NT_ClearEOD();
			break;
		case BL:
			MessageBeep(MB_ICONHAND);
			break;
		case CE:
			nt_ClearEOL();
			break;
		case CL:
			NT_ClearScreen();
			break;
		case KB:
			nt_write(fd,&c,1);
			break;
		case LE:
			NT_MoveToLineOrChar(-1,0);
			break;
		case ND:
			NT_MoveToLineOrChar(1,0);
			break;
		case UPPER_RI:
			mult_buf[curr_pos++] = UPPER_RI;
			state = PARSING_MULT;
			break;
		case UPPER_LE:
			mult_buf[curr_pos++] = UPPER_LE;
			state = PARSING_MULT;
			break;
		case DO:
			NT_MoveToLineOrChar(1,1);
			break;
		case KU:
			NT_MoveToLineOrChar(-1,1);
			break;
		case UP:
			NT_MoveToLineOrChar(-1,1);
			break;
		case TA:
			nt_move_next_tab();
			break;
		case 'Z':
			tmp = &mult_buf[1];
			mult_buf[curr_pos]=0;
			rc = atoi(tmp);
			move_cursor(mult_buf[0],rc);
			rc = 0;
			curr_pos=0;
			state = 0;
			break;
		default:
			nt_write(fd,&c,1);
			break;
	}
	return rc;
}
int tc_putfunc(char c) {
	extern int SHOUT;
	return tc_fd_putc(c,SHOUT);
}
int move_cursor(char c,int howmany) {
	switch (c) {
		case  UPPER_UP: //up
			NT_MoveToLineOrChar(-howmany,1);
			break;
		case  UPPER_DO: //down
			NT_MoveToLineOrChar(howmany,1);
			break;
		case  UPPER_RI: //right
			NT_MoveToLineOrChar(howmany,0);
			break;
		case  UPPER_LE: //left
			NT_MoveToLineOrChar(-howmany,0);
			break;
		default :
			break;
	}
	return 0;
}
