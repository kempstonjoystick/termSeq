/*
 * termSeq.h
 *
 *  Created on: Jun 15, 2015
 *      Author: timmins
 */

#ifndef INCLUDES_TERMSEQ_H_
#define INCLUDES_TERMSEQ_H_

#include <stdarg.h>
#include <stdio.h>

void dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

#define DEBUG 1

#define TRACE(x) do { if (DEBUG) dbg_printf x; } while (0)


/* Tests itself using predefined test files */
#define UNIT_TEST

#ifdef UNIT_TEST

#endif


#endif /* INCLUDES_TERMSEQ_H_ */
