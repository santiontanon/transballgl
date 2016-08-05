
#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include <stdlib.h>
#include "stdarg.h"


FILE *fp=0;

void output_debug_message(const char *fmt, ...)						
{
    char text[256], fname[256];
    va_list ap;
	
    if (fmt == 0)
        return ;
	
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);
	
#ifdef __EMSCRIPTEN__
    printf("%s",text);
#else
    if (fp == 0) {
#ifdef _WIN32
        sprintf(fname, "transballGL.dbg");	/* 
		 In Visual C++ 6, "snprint" is not defined, it was just recently added to 
		 the ANSI standard, so I've replaced it with the unsafer but more
		 compatible "sprintf". If anyone knows any way of using "ifdef"s to detect
		 whether snprintf is defined or not, and use the safer one. I'd appreciate it.
		 */ 
#else
        snprintf(fname, 255, "%s/transballGL.dbg", getenv("HOME"));
#endif
        fp = fopen(fname, "w");
    }
	
    fprintf(fp, "%s",text);
    fflush(fp);
#endif
} /* glprintf */




void close_debug_messages(void)
{
#ifdef __EMSCRIPTEN__
#else
    fclose(fp);
    fp=0;
#endif
} /* close_debug_messages */
