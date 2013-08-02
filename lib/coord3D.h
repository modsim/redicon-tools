/*  Coord3D.h  2013-07-29  Coord Class Header File 
 *
 * Copyright (C) 2012 Svyatoslav Kondrat (Valiska)
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

#if !defined ( __HAVE_COORD3D_H__ )
#  define __HAVE_COORD3D_H__

#include <stdio.h>
#include <iostream> 
#include <fstream>

class Coord3D
{
	public:
		Coord3D (double x, double y, double z);
		Coord3D (double Coord[3]);
		Coord3D (const Coord3D &); // copy-constructor

		~Coord3D() {};

		double getCoord (unsigned int i) const;
		void getCoord (double *) const;

		// set Coords
		bool setCoord (unsigned int i, double val);
		bool setCoord (const double val[3]); // no checks, use with caution

		// Operations
		double norm () const;
		double distance (const Coord3D &a) const;

		// Print Coords
		void print (FILE * file, const char * name) const ;
		void print (FILE * file, const char * name, unsigned int i) const;

		//void print (std::ofstream *, const char * name) const;
		//void print (std::ofstream *, const char * name, int i) const;

		void print (std::ostream *) const;

	private:
		unsigned int dim ; // must be 3
		double r[3];
};

#endif /* _HAVE_ILP_INDEX_H_ */

