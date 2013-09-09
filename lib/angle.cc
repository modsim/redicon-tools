/*  angle.cc 2013-08-23 valiska@gmail.com
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

#include <stddef.h>
#include <string.h>
#include <iostream> 
#include <math.h> 

#include<angle.h>

#include "defines.h"

#define DEPS    0.0

//
//  Constructors
//
Angle::Angle (Atom * a1, Atom * a2, Atom * a3, AnglePotType type, double H, double alpha0) : 
		type(type), alpha0(alpha0), H(H) 
{
	if (!atomsOK (*a1, *a2, *a3))
		throw "Cannot create a bond angle.";

	a[0] = a1; a[1] = a2; a[2] = a3;
	typestr = getTypeStr();
	double alpha = 0.;
	if ( calcAngle(&alpha) && alpha != alpha0)
	{
		BCPT_WARNING ("angle determined from atom's position %f differs from the specified angle %f",
			alpha, alpha0);
	}

	AtomAttorney::addAngle (*a1, *this);
	AtomAttorney::addAngle (*a2, *this);
	AtomAttorney::addAngle (*a3, *this);
};

Angle::Angle (Atom * a1, Atom * a2, Atom * a3, AnglePotType type, double H) : type(type), H(H)
{
	if (!atomsOK (*a1, *a2, *a3))
		throw "Cannot create a bond angle.";

	a[0] = a1; a[1] = a2; a[2] = a3;
	typestr = getTypeStr();
	calcAngle(&alpha0);

	AtomAttorney::addAngle (*a1, *this);
	AtomAttorney::addAngle (*a2, *this);
	AtomAttorney::addAngle (*a3, *this);
};

Angle::Angle (Atom * a1, Atom * a2, Atom * a3, double H) : type(ANGLE_POTTYPE_SQUARE), H(H)
{
	if (!atomsOK (*a1, *a2, *a3))
		throw "Cannot create a bond angle.";

	a[0] = a1; a[1] = a2; a[2] = a3;
	typestr = getTypeStr();
	calcAngle(&alpha0);

	AtomAttorney::addAngle (*a1, *this);
	AtomAttorney::addAngle (*a2, *this);
	AtomAttorney::addAngle (*a3, *this);
};

Angle::Angle (Atom * a1, Atom * a2, Atom * a3, AnglePotType type) : type(type), H(1.0)
{
	if (!atomsOK (*a1, *a2, *a3))
		throw "Cannot create a bond angle.";

	a[0] = a1; a[1] = a2; a[2] = a3;
	typestr = getTypeStr();
	calcAngle(&alpha0);

	AtomAttorney::addAngle (*a1, *this);
	AtomAttorney::addAngle (*a2, *this);
	AtomAttorney::addAngle (*a3, *this);
};

Angle::Angle (Atom * a1, Atom * a2, Atom * a3) : type(ANGLE_POTTYPE_SQUARE), H(1.0)
{
	if (!atomsOK (*a1, *a2, *a3))
		throw "Cannot create a bond angle.";

	a[0] = a1; a[1] = a2; a[2] = a3;
	typestr = getTypeStr();
	calcAngle(&alpha0);

	AtomAttorney::addAngle (*a1, *this);
	AtomAttorney::addAngle (*a2, *this);
	AtomAttorney::addAngle (*a3, *this);
};

const std::string Angle::getTypeStr () 
{
	if (type == ANGLE_POTTYPE_SQUARE)
		return "angle";
	else if (type == ANGLE_POTTYPE_COSINE)
		return "cos";
	else 
	{
		BCPT_ERROR ("unknown or unsupported type");
		return NULL;
	}
}

//
// Check if atoms are ok for an angle
bool Angle::atomsOK (const Atom &a1, const Atom &a2, const Atom &a3) const
{
	if (&a1 == &a2 || &a2 == &a3 || &a3 == &a1)
	{
		BCPT_ERROR ("Cannot create a bond angle between the same atoms.");
		return false;
	}

	if (a1.getSerial() == a2.getSerial() || a2.getSerial() == a3.getSerial() || a3.getSerial() == a1.getSerial())
	{
		BCPT_ERROR ("Cannot create a bond angle between atoms with the same serial number.");
		return false;
	}
	return true;
}

//
// Calculate current angle 
bool Angle::calcAngle (double * alpha) const
{
	BCPT_RET_VAL_IF_FAIL (alpha, false);

	if ( a[0]->positionIsSet() && a[1]->positionIsSet() && a[2]->positionIsSet() )
	{ 
		const Point3D * p1 = a[0]->positionPtr();
		const Point3D * p2 = a[1]->positionPtr();
		const Point3D * p3 = a[2]->positionPtr();
		
		Point3D A = p1->vectorTo (*p2);
		Point3D B = p2->vectorTo (*p3);
		*alpha = A.angleBetween (B);

		if (*alpha == 0.0)
		{
			BCPT_WARNING ("angle is zero");
		}
		return 	true;
	}
	else
	{
		BCPT_ERROR ("cannot get the 'ground state' angle, setting to 180.");
		*alpha = M_PI;
		return false;
	}
}

// For each 'loop'
bool Angle::foreachAtom (AtomFunction func, void * data) const
{
	for (int i = 0; i < 3; i++)
		if (!func(*a[i], data))
			return false;
	return true;
}

// Check atoms
bool Angle::hasAtom (const Atom * a) const
{
	for (int i = 0; i < 3; i++)
		if ( (a == Angle::a[i]) )
			return true;

	return false;
}

//
// print Molecule's info
//
void Angle::printInfo (char * name) const
{
	std::ostream * stream = new std::ofstream (name);
	printInfo (stream);
	delete stream;
}

void Angle::printInfo (std::ostream * stream) const
{
	*stream << "Bond angle between" ;
	for (int i = 0; i < 3; i++)
		*stream <<  " '" << a[i]->getName() << "' " ;
	*stream  << std::endl ;
}

void Angle::printBBStr (std::ostream * stream) const
{
	*stream << "angle " << typestr;
	for (int i = 0; i < 3; i++)
		*stream <<  "  " << a[i]->getSerial();

	*stream << "   " << AngleOptimal() << " " << getH() << std::endl ;
}

