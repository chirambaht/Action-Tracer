#ifndef _DEBUG_PRINTER_H_
#define _DEBUG_PRINTER_H_

#include <stdio.h>

#define DEBUG 1

#if DEBUG == 1
	#define debugPrint( ... )	 printf( __VA_ARGS__ )
	#define debugPrintln( ... )	 printf( __VA_ARGS__ )
	#define fdebugPrint( ... )	 fprintf( __VA_ARGS__ )
	#define fdebugPrintln( ... ) fprintf( __VA_ARGS__ )
#else
	#define debugPrint( ... )
	#define debugPrintln( ... )
	#define fdebugPrint( ... )
	#define fdebugPrintln( ... )
#endif

#endif