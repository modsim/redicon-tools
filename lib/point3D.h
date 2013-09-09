/*  Point3D.h  2013-07-29  3D Point Class Header File 
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

#if !defined ( __HAVE_POINT3D_H__ )
#  define __HAVE_POINT3D_H__

#include <stdio.h>
#include <iostream> 
#include <fstream>

class Point3D;
typedef Point3D Coord3D; 

class Point3D
{
	public:
		Point3D (double x, double y, double z);
		Point3D (double Point[3]);
		Point3D (const Point3D &); // copy-constructor

		~Point3D() {};

		double get (unsigned int i) const;
		void get (double *) const;

		// set Points
		bool set (unsigned int i, double val);
		bool set (const double val[3]); // no checks, use with caution

		// Operations
		double norm () const;
		double distanceTo (const Point3D &b) const;
		Point3D vectorTo (const Point3D &b) const; // vector to a point b
		double angleBetween (const Point3D &b) const ; // angle between me as a vector and vector b

		void translateBy (const Point3D &b) ; // translate me by a vector b

		// Print Points
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

