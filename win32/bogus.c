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
 * 3. Neither the name of the University nor the names of its contributors
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
 * bogus.c: various routines that are really silly
 * -amol
 *
 */
#include "ntport.h"
#include "sh.h"

static struct passwd pass_bogus;
static char username[20];
static char homedir[256];
static char *this_shell="tcsh";

static char dummy[2]={0,0};

gid_t getuid(void) {
	return 0;
}
gid_t getgid(void) {
	return 0;
}
gid_t geteuid(void) {
	return 0;
}
gid_t getegid(void) {
	return 0;
}
struct passwd * getpwnam(char *name) {

	char *ptr;
	int size =20;

	if (pass_bogus.pw_name == NULL) {
		GetUserName(username,&size);
		ptr = getenv("HOME");
		if (ptr){
			strcpy(homedir,ptr);
			pass_bogus.pw_dir = &homedir[0];
		}
		pass_bogus.pw_name = &username[0];
		pass_bogus.pw_shell = this_shell;
		

		pass_bogus.pw_passwd= &dummy[0];
		pass_bogus.pw_gecos=&dummy[0];
		pass_bogus.pw_passwd= &dummy[0];
		
	}
	if (stricmp(username,name) )
		return NULL;
	return &pass_bogus;
}
struct passwd * getpwuid(uid_t uid) {

	char *ptr;
	int size =20;

	if (pass_bogus.pw_name == NULL) {
		GetUserName(username,&size);
		ptr = getenv("HOME");
		if (ptr){
			strcpy(homedir,ptr);
			pass_bogus.pw_dir = &homedir[0];
		}
		pass_bogus.pw_name = &username[0];
		pass_bogus.pw_shell = this_shell;
		

		pass_bogus.pw_passwd= &dummy[0];
		pass_bogus.pw_gecos=&dummy[0];
		pass_bogus.pw_passwd= &dummy[0];
		
	}
	return &pass_bogus;
}
struct group * getgrnam(char *name) {

	return NULL;
}
struct group * getgrgid(gid_t gid) {

	return NULL;
}
char * ttyname(int fd) {

	if (isatty(fd)) return "/dev/tty";
	return NULL;
}
int times(struct tms * ignore) {
	FILETIME c,e,kernel,user;
	
	ignore->tms_utime=0;
	ignore->tms_stime=0;
	ignore->tms_cutime=0;
	ignore->tms_cstime=0;
	if (!GetProcessTimes(GetCurrentProcess(),
						&c,
						&e,
						&kernel,
						&user) )
		return -1;

	if (kernel.dwHighDateTime){
		return GetTickCount();
	}
	//
	// Units of 10ms. I *think* this is right. -amol 6/2/97
	ignore->tms_stime = kernel.dwLowDateTime / 1000 /100;
	ignore->tms_utime = user.dwLowDateTime / 1000 /100;
	
	return GetTickCount();
}
int tty_getty(int fd, void*ignore) {
	return 0;
}
int tty_setty(int fd, void*ignore) {
	return 0;
}
int tty_geteightbit(void *ignore) {
	return 1;
}
void
dosetty(v, t)
    Char **v;
    struct command *t;
{
	xprintf("setty not supported in NT\n");
}

