#ifndef _DEBUG_PRINTER_H_
#define _DEBUG_PRINTER_H_

#include <stdio.h>

#define DEBUG 0

#if DEBUG == 1
#define debugPrint(...) printf(__VA_ARGS__)
#define debugPrintln(...) printf(__VA_ARGS__)
#define fdebugPrint(...) fprintf(__VA_ARGS__)
#define fdebugPrintln(...) fprintf(__VA_ARGS__)
#else
#define debugPrint(x)
#define debugPrintln(x)
#define fdebugPrint(x)
#define fdebugPrintln(x)
#endif

#endif