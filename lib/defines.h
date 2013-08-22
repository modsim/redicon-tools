/*  defines.h  2013-07-26
 *
 * Copyright (C) 2007 Svyatoslav Kondrat (Valiska)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#if !defined ( _HAVE_DEFINES_H_)
#define _HAVE_DEFINES_H_

#include <stdio.h>
#include <stdlib.h>

#define pow2(x)         ( (x)*(x) )

#define DBLEPS     1.0e-16
#define BCPPDB_DBL_PRECISION   1.0e-03 /* we read only 6 digids in PDB */

/*********************************************
**********************************************/

#define BCPT_WARNING(str...)     { fprintf(stderr, "\n*** BCP TOOLS WARNING: %s():", __ASSERT_FUNCTION); \
		    fprintf(stderr, " " str); fprintf(stderr,"\n\n"); fflush(stderr); }


#define BCPT_ERROR(str...)     { fprintf(stderr, "\n*** BCP TOOLS ERROR: %s():", __ASSERT_FUNCTION); \
		    fprintf(stderr, " " str); fprintf(stderr,"\n\n"); fflush(stderr); }

/* Critical errors/assertions which depend on user (given data, functions, etc) */
#define BCPT_ASSERT(cond,str...) { \
  if (!(cond)) {\
    fprintf(stderr, "\n*** BCP TOOLS CRITICAL ERROR: " str);\
    fprintf(stderr, " aborting.\n");\
    fflush(stderr);\
    abort();\
  } }


#define BCPT_RET_VAL_IF_FAIL(cond,val) { \
  if (!(cond)) {\
    fprintf(stderr, "\n*** BCP TOOLS ERROR: %s: '%s` assertion failed\n", \
	     __ASSERT_FUNCTION, __STRING(cond) );\
    fflush(stderr);\
    return val; } \
  }

#define BCPT_RET_IF_FAIL(cond) { \
  if (!(cond)) {\
    fprintf(stderr, "\n*** BCP TOOLS ERROR: %s: '%s` assertion failed\n", \
	     __ASSERT_FUNCTION, __STRING(cond) );\
    fflush(stderr);\
    return; } \
  }

#ifdef BCPT_NO_INTERNAL_ASSERT

  #define BCPT_ASSERT_INTERNAL(cond) 

#else 

/* Errors/assertions which should not happen */
#include <assert.h>

  #define BCPT_ASSERT_INTERNAL(cond) { \
    if (!(cond)) {\
    fprintf(stderr, "\n*** BCP TOOLS INTERNAL ERROR: %s:%d: %s(): assertion `%s' failed, aborting.\n"\
		, __FILE__, __LINE__, __ASSERT_FUNCTION, __STRING(cond) );\
    fflush(stderr);\
    abort();\
  } }

#endif

/* For debugging */
#ifdef DEBUG

  #define DPRINT(str...) fprintf(stderr, str); fflush(stderr);

  #define DPRINT_VECTOR(s, q, n)  {\
    int zi ; \
    for (i = 0; i < n; i++) { \
      fprintf (stderr, "%s[%d]=%f\n", s, zi, q[zi]); fflush(stderr);} \
      fprintf(stderr, "\n"); fflush(stderr);\
    }

#else

  #define DPRINT(str...)

  #define DPRINT_VECTOR(s, q, n)

#endif /* DEBUG */

#endif /* _HAVE_DEFINES_H_ */ 
