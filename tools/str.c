/*  str.c:  some string utils (2013-07-31 from 2010-10-04 from 2007-11-19)
 *
 * Copyright (C) 2007 2010 2013 Svyatoslav Kondrat (Valiska)
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

#include "str.h"

//#define DEBUG
#include "defines.h"

unsigned int str2dlist (char ** string, const char * sep, double ** list) {

  int i = 0;
  char * str = (char*) NULL;

  char * orig_string = *string;

  DPRINT ("str2dlist(): string=\"%s\"   sep=\"%s\"\n", *string, sep);

  while (1)  {

#ifdef HAVE_STRSEP
    str = strsep (string, sep);
#else
    str = *string;
    str = strtok_r (str, sep, string);
#endif  /* HAVE_STRSEP */

    DPRINT ("str2dlist(): parsed: \"%s\"\n", str);

    if (str == NULL) 
      break;
    else {
      *list = (double*) realloc (*list, (i+1) * sizeof (double));
      (*list)[i] = atof (str);
    }
    DPRINT ("str2dlistt(): value: %g\n", (*list)[i]);
    i++;

  }

  DPRINT ("str2dlist(): n=%i\n", i);

  *string = orig_string;

  return i;

}

char ** str2strlist (char ** string, const char * sep, unsigned int * n) {

  int i = 0;
  char * str = (char*) NULL;
  char ** list = (char**) NULL;
  char * orig_string = *string;

  DPRINT ("str2strlist(): \"%s\"\n", *string);

  while (1)  {

#ifdef HAVE_STRSEP
    str = strsep (string, sep);
#else
    str = *string;
    str = strtok_r (str, sep, string);
#endif  /* HAVE_STRSEP */

    DPRINT ("str2strlist(): parsed: \"%s\"\n", str);

    if (str == NULL) 
      break;
    else {
      list = (char**) realloc (list, (i+1) * sizeof (char*));
      list[i] = strdup (str);
      DPRINT ("str2strlist(): str[%i]: \"%s\"\n", i, str);
    }
    i++;

  }

  DPRINT ("str2strlist(): n=%i\n", i);
  if (n) *n = i;

  *string = orig_string;

  return list;

}

