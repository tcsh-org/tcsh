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
 * globals.c: The mem locations needed in the child are copied here.
 * -amol
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

extern unsigned long bookend1,bookend2;
extern char **environ;

#undef dprintf
void
dprintf(char *format, ...)
{				/* } */
	va_list vl;
	char putbuf[2048];
	DWORD err;

	err = GetLastError();
	{
		va_start(vl, format);
		_vsnprintf(putbuf, sizeof(putbuf),format, vl);
		va_end(vl);
		OutputDebugString(putbuf);
	}
	SetLastError(err);
}
/*
 * This function is called by fork(). The process must copy
 * whatever memory is needed in the child. hproc is a handle
 * to the child process
 *
 */
int fork_copy_user_mem(HANDLE hproc) {
	
	DWORD bytes,rc;
	size_t size;

	size =(char*)&bookend2 - (char*)&bookend1;
	rc =WriteProcessMemory(hproc,&bookend1,&bookend1,
					(DWORD)size,
					&bytes);

	if (!rc) {
		rc = GetLastError();
		return -1;
	}
	if (size != bytes) {
		//dprintf("size %d , wrote %d\n",size,bytes);
	}
	return 0;
}
/*
How To Determine Whether an Application is Console or GUI     [win32sdk]
ID: Q90493     CREATED: 15-OCT-1992   MODIFIED: 16-DEC-1996
*/
#include <winnt.h>
#include <ntport.h>

#define XFER_BUFFER_SIZE 2048

int is_gui(char *exename) {

	HANDLE hImage;

	DWORD  bytes;
	DWORD  SectionOffset;
	DWORD  CoffHeaderOffset;
	DWORD  MoreDosHeader[16];

	ULONG  ntSignature;

	IMAGE_DOS_HEADER      image_dos_header;
	IMAGE_FILE_HEADER     image_file_header;
	IMAGE_OPTIONAL_HEADER image_optional_header;


	hImage = CreateFile(exename, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hImage) {
		return 0;
	}

	/*
	 *  Read the MS-DOS image header.
	 */
	if (!ReadFile(hImage, &image_dos_header, sizeof(IMAGE_DOS_HEADER),
			&bytes,NULL)){
		CloseHandle(hImage);
		return 0;
	}

	if (IMAGE_DOS_SIGNATURE != image_dos_header.e_magic) {
		CloseHandle(hImage);
		return 0;
	}

	/*
	 *  Read more MS-DOS header.       */
	if (!ReadFile(hImage, MoreDosHeader, sizeof(MoreDosHeader),
			&bytes,NULL)){
		CloseHandle(hImage);
		return 0;
	}

	/*
	 *  Get actual COFF header.
	 */
	CoffHeaderOffset = SetFilePointer(hImage, image_dos_header.e_lfanew,
			NULL,FILE_BEGIN);

	if (CoffHeaderOffset == (DWORD) -1){
		CloseHandle(hImage);
		return 0;
	}

	CoffHeaderOffset += sizeof(ULONG);

	if (!ReadFile (hImage, &ntSignature, sizeof(ULONG),
			&bytes,NULL)){
		CloseHandle(hImage);
		return 0;
	}

	if (IMAGE_NT_SIGNATURE != ntSignature) {
		CloseHandle(hImage);
		return 0;
	}

	SectionOffset = CoffHeaderOffset + IMAGE_SIZEOF_FILE_HEADER +
		IMAGE_SIZEOF_NT_OPTIONAL_HEADER;

	if (!ReadFile(hImage, &image_file_header, IMAGE_SIZEOF_FILE_HEADER,
			&bytes, NULL)){
		CloseHandle(hImage);
		return 0;
	}

	/*
	 *  Read optional header.
	 */
	if (!ReadFile(hImage, &image_optional_header, 
			IMAGE_SIZEOF_NT_OPTIONAL_HEADER,&bytes,NULL)) {
		CloseHandle(hImage);
		return 0;
	}

	CloseHandle(hImage);

	if (image_optional_header.Subsystem ==IMAGE_SUBSYSTEM_WINDOWS_GUI)
		return 1;
	return 0;
}
int is_9x_gui(char *prog) {
	
	char *progpath;
	DWORD dwret;
	char *pathbuf;
	char *pext;
	
	pathbuf=heap_alloc(MAX_PATH);

	progpath=heap_alloc(MAX_PATH<<1);

	if (GetEnvironmentVariable("PATH",pathbuf,MAX_PATH) ==0) {
		goto failed;
	}
	
	pathbuf[MAX_PATH]=0;

	dwret = SearchPath(pathbuf,prog,".EXE",MAX_PATH<<1,progpath,&pext);

	if ( (dwret == 0) || (dwret > (MAX_PATH<<1) ) )
		goto failed;
	
	dprintf("progpath is %s\n",progpath);
	dwret = is_gui(progpath);

	heap_free(pathbuf);
	heap_free(progpath);

	return dwret;

failed:
	heap_free(pathbuf);
	heap_free(progpath);
	return 0;


}
