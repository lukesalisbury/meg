/*  Pawn compiler driver
 *
 *  Function and variable definition and declaration, statement parser.
 *
 *  Copyright (c) ITB CompuPhase, 2006-2009
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *  use this file except in compliance with the License. You may obtain a copy
 *  of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *  License for the specific language governing permissions and limitations
 *  under the License.
 *
 *  Version: $Id: pawncc.c 4125 2009-06-15 16:51:06Z thiadmer $
 */
#include <limits.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if PAWN == 4
    #include "pawn4/sc.h"
#else
    #include "pawn3/sc.h"
#endif

#include <glib.h>
#include <gmodule.h>


int (*error_callback)(int number, char * message, char *filename, int firstline, int lastline, va_list argptr);


/* pc_printf()
 * Called for general purpose "console" output. This function prints general
 * purpose messages; errors go through pc_error(). The function is modelled
 * after printf().
 */
int pc_printf(const char *message,...)
{
    /*
  int ret;
  va_list argptr;

  va_start(argptr,message);
  ret=vprintf(message,argptr);
  va_end(argptr);
  return ret;
*/
  return 0;
}

/* pc_error()
 * Called for producing error output.
 *    number      the error number (as documented in the manual)
 *    message     a string describing the error with embedded %d and %s tokens
 *    filename    the name of the file currently being parsed
 *    firstline   the line number at which the expression started on which
 *                the error was found, or -1 if there is no "starting line"
 *    lastline    the line number at which the error was detected
 *    argptr      a pointer to the first of a series of arguments (for macro
 *                "va_arg")
 * Return:
 *    If the function returns 0, the parser attempts to continue compilation.
 *    On a non-zero return value, the parser aborts.
 */
int pc_error(int number, char * message, char *filename, int firstline,int lastline, va_list argptr)
{
    if ( error_callback )
        error_callback(number, message, filename,firstline,lastline, argptr );
    return 0;
}

/* pc_opensrc()
 * Opens a source file (or include file) for reading. The "file" does not have
 * to be a physical file, one might compile from memory.
 *    filename    the name of the "file" to read from
 * Return:
 *    The function must return a pointer, which is used as a "magic cookie" to
 *    all I/O functions. When failing to open the file for reading, the
 *    function must return NULL.
 * Note:
 *    Several "source files" may be open at the same time. Specifically, one
 *    file can be open for reading and another for writing.
 */
void *pc_opensrc(char *filename)
{
  return fopen(filename,"rt");
}

/* pc_createsrc()
 * Creates/overwrites a source file for writing. The "file" does not have
 * to be a physical file, one might compile from memory.
 *    filename    the name of the "file" to create
 * Return:
 *    The function must return a pointer, which is used as a "magic cookie" to
 *    all I/O functions. When failing to open the file for reading, the
 *    function must return NULL.
 * Note:
 *    Several "source files" may be open at the same time. Specifically, one
 *    file can be open for reading and another for writing.
 */
void *pc_createsrc(char *filename)
{
  return fopen(filename,"wt");
}

/* pc_closesrc()
 * Closes a source file (or include file). The "handle" parameter has the
 * value that pc_opensrc() returned in an earlier call.
 */
void pc_closesrc(void *handle)
{
  assert(handle!=NULL);
  fclose((FILE*)handle);
}

/* pc_readsrc()
 * Reads a single line from the source file (or up to a maximum number of
 * characters if the line in the input file is too long).
 */
char *pc_readsrc(void *handle,unsigned char *target,int maxchars)
{
  return fgets((char*)target,maxchars,(FILE*)handle);
}

/* pc_writesrc()
 * Writes to to the source file. There is no automatic line ending; to end a
 * line, write a "\n".
 */
int pc_writesrc(void *handle,const unsigned char *source)
{
  return fputs((char*)source,(FILE*)handle) >= 0;
}

#define MAXPOSITIONS  4
static fpos_t srcpositions[MAXPOSITIONS];
static unsigned char srcposalloc[MAXPOSITIONS];

void pc_clearpossrc(void)
{
  memset(srcpositions,0,sizeof srcpositions);
  memset(srcposalloc,0,sizeof srcposalloc);
}

void *pc_getpossrc(void *handle,void *position)
{
  if (position==NULL) {
    /* allocate a new slot */
    int i;
    for (i=0; i<MAXPOSITIONS && srcposalloc[i]!=0; i++)
      /* nothing */;
    assert(i<MAXPOSITIONS); /* if not, there is a queue overrun */
    if (i>=MAXPOSITIONS)
      return NULL;
    position=&srcpositions[i];
    srcposalloc[i]=1;
  } else {
    /* use the gived slot */
    assert((fpos_t*)position>=srcpositions && (fpos_t*)position<srcpositions+sizeof(srcpositions));
  } /* if */
  fgetpos((FILE*)handle,(fpos_t*)position);
  return position;
}

/* pc_resetsrc()
 * "position" may only hold a pointer that was previously obtained from
 * pc_getpossrc()
 */
void pc_resetsrc(void *handle,void *position)
{
  assert(handle!=NULL);
  assert(position!=NULL);
  fsetpos((FILE*)handle,(fpos_t*)position);
  /* note: the item is not cleared from the pool */
}

int pc_eofsrc(void *handle)
{
  return feof((FILE*)handle);
}

/* should return a pointer, which is used as a "magic cookie" to all I/O
 * functions; return NULL for failure
 */
void *pc_openasm(char *filename)
{
  #if defined __MSDOS__ || defined PAWN_LIGHT
    return fopen(filename,"w+t");
  #else
    return mfcreate(filename);
  #endif
}

void pc_closeasm(void *handle, int deletefile)
{
  #if defined __MSDOS__ || defined PAWN_LIGHT
    if (handle!=NULL)
      fclose((FILE*)handle);
    if (deletefile)
      remove(outfname);
  #else
    if (handle!=NULL) {
      if (!deletefile)
        mfdump((MEMFILE*)handle);
      mfclose((MEMFILE*)handle);
    } /* if */
  #endif
}

void pc_resetasm(void *handle)
{
  assert(handle!=NULL);
  #if defined __MSDOS__ || defined PAWN_LIGHT
    fflush((FILE*)handle);
    fseek((FILE*)handle,0,SEEK_SET);
  #else
    mfseek((MEMFILE*)handle,0,SEEK_SET);
  #endif
}

int pc_writeasm(void *handle,const char *string)
{
  #if defined __MSDOS__ || defined PAWN_LIGHT
    return fputs(string,(FILE*)handle) >= 0;
  #else
    return mfputs((MEMFILE*)handle,string);
  #endif
}

char *pc_readasm(void *handle, char *string, int maxchars)
{
  #if defined __MSDOS__ || defined PAWN_LIGHT
    return fgets(string,maxchars,(FILE*)handle);
  #else
    return mfgets((MEMFILE*)handle,string,maxchars);
  #endif
}

/* Should return a pointer, which is used as a "magic cookie" to all I/O
 * functions; return NULL for failure.
 */
void *pc_openbin(char *filename)
{
  return fopen(filename,"wb");
}

void pc_closebin(void *handle,int deletefile)
{
  fclose((FILE*)handle);
  if (deletefile)
    remove(binfname);
}

/* pc_resetbin()
 * Can seek to any location in the file.
 * The offset is always from the start of the file.
 */
void pc_resetbin(void *handle,long offset)
{
  fflush((FILE*)handle);
  fseek((FILE*)handle,offset,SEEK_SET);
}

int pc_writebin(void *handle,const void *buffer,int size)
{
  return (int)fwrite(buffer,1,size,(FILE*)handle) == size;
}

long pc_lengthbin(void *handle)
{
  return ftell((FILE*)handle);
}


#ifdef __GNUWIN32__
#include <windows.h>
BOOL APIENTRY DllMain ( HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
    #if PAWN == 4
    #define DLL_EXPORT
    #else
    #define DLL_EXPORT __declspec(dllexport)
    #endif
#else
#define DLL_EXPORT
#endif


DLL_EXPORT int compiler_main( int argc, char *argv[], int (*error_handling)(int number, char * message, char *filename, int firstline, int lastline, va_list argptr) )
{

    error_callback = error_handling;
	return pc_compile(argc, argv);
}
