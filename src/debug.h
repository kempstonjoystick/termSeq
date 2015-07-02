/*
 * debug.h
 *
 *  Created on: Jul 1, 2015
 *      Author: timmins
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdarg.h>
#include <stdio.h>

static void dbug_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

#define DEBUG 1

#define TRACE(x) do { if (DEBUG) dbug_printf x; } while (0)



#endif /* DEBUG_H_ */
