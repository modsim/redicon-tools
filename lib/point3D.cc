/*  Point3D.cc  2013-07-29 Point base class source code
 *
 * Copyright (C) 2013 Svyatoslav Kondrat (Valiska)
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

#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "point3D.h"

//#define DEBUG
#include "defines.h"

//
// Constructors
//
Point3D::Point3D (double x, double y, double z) : dim(3)
{
	r[0] = x; r[1] = y; r[2] = z;
}

// from prealloc'ed double*
Point3D::Point3D (double R[3]) : dim(3)
{
	memcpy (this->r, R, dim * sizeof (double) );
}

// Copy-constructor
Point3D::Point3D (const Point3D & R) : dim(3)
{
	R.get (this->r);
}

//
// Get coords
//
double Point3D::get (unsigned int i) const
{
	BCPT_RET_VAL_IF_FAIL (i<dim, 0.0);
	return r[i];
}
void Point3D::get (double * R) const
{
	BCPT_RET_IF_FAIL (r);
	memcpy (R, r, dim * sizeof (double));
}

//
// Set Points
//
bool Point3D::set (unsigned int i, double val)
{
	BCPT_RET_VAL_IF_FAIL (i<dim, false);
	r[i] = val;
	return true;
}
bool Point3D::set (const double * val)
{
	BCPT_RET_VAL_IF_FAIL (val, false);
	memcpy (this->r, val, dim * sizeof (double) );
	return true;
}

//
// Operations
//
double Point3D::norm () const
{
	double r2 = 0;
	for (unsigned int i = 0; i < dim; i++)
		r2 += r[i] * r[i];

	return sqrt (r2);
}

double Point3D::distanceTo (const Point3D & a) const
{
	double r2 = 0.0;
	for (unsigned int i = 0; i < dim; i++)
		r2 += pow2(r[i] - a.get(i));

	return sqrt (r2);
}
Point3D Point3D::vectorTo (const Point3D & a) const
{
	double R[3];
	for (unsigned int i = 0; i < dim; i++)
		R[i] = a.get(i) - r[i];

	Point3D p (R);
	return p;
}

double Point3D::angleBetween (const Point3D & a) const
{
	double prod = 0.;
	for (unsigned int i = 0; i < dim; i++)
		prod += a.get(i) * r[i];

	prod /= (a.norm() * norm());

	DPRINT ("product=%f, alpha=%f\n", prod, acos (prod));
	return acos (prod);
}

void Point3D::translateBy (const Point3D & a)
{
	for (unsigned int i = 0; i < dim; i++)
		r[i] += a.get(i);
}

//
// Print coords
//
void Point3D::print (FILE * file, const char * name) const
{
	BCPT_RET_IF_FAIL (file);

	for (unsigned int i = 0; i < dim; i++)
	{
		if (name)
			fprintf (file, "%s[%i] = %g\n", name, i, r[i]);
		else
			fprintf (file, " %e ", r[i]);
	}
	return;
}

void Point3D::print (FILE * file, const char * name, unsigned int i) const
{
	BCPT_RET_IF_FAIL (file);
	BCPT_RET_IF_FAIL (i < dim);

	if (name)
		fprintf (file, "%s[%i] = %g\n", name, i, r[i]);
	else
		fprintf (file, " %e ", r[i]);
	return;
}

void Point3D::print (std::ostream * stream) const
{
	*stream << "  " << r[0] << " " << r[1] << " " <<  r[2] << "  ";
}

