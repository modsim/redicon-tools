/*  str.h  --  some string utils (from D2L.c of f3dm) --  2007-11-19
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

#if !defined _HAVE_STR_H
#define _HAVE_STR_H

#undef __BEGIN_DECLS
#undef __END_DECLS

#ifdef __cplusplus
#   define __BEGIN_DECLS extern "C" {
#   define __END_DECLS }
#else
#   define __BEGIN_DECLS /* empty */
#   define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

unsigned int str2dlist (char ** string, const char * sep, double ** list);
char ** str2strlist (char ** string, const char * sep, unsigned int * n) ;

__END_DECLS

#endif

