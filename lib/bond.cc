/*  bond.cc 2013-08-05 valiska@gmail.com
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

#include<bond.h>

#include "defines.h"

Bond::Bond (Atom * a, Atom * b) : a(a), b(b), userData (NULL)
{
	if (a == b)
		throw "Cannot create a bond between the same atom.";

	if (a->positionIsSet() && b->positionIsSet())
	{ // try to get bond length & set rigid bond
		const Point3D * p1 = a->positionPtr();
		const Point3D * p2 = b->positionPtr();
		rmin = rmax = p1->distanceTo (*p2);
		H = 1.;
	//	BCPT_WARNING ("setting rigid bond length to %g", rmin);
	}
	else
	{
		BCPT_WARNING ("cannot get bond length, setting rigid bond to zero");
		rmin = rmax = H = 0.0;
	}

	AtomAttorney::setType (*a, ATOM_BONDED);
	AtomAttorney::setType (*b, ATOM_BONDED);

	AtomAttorney::addBond (*a, *this);
	AtomAttorney::addBond (*b, *this);
};

// For each 'loop'
bool Bond::foreachAtom (AtomFunction func, void * data) const
{
	if (!func(*a, data))
		return false;
	if (!func(*b, data))
		return false;

	return true;
}

// Check atoms
bool Bond::hasAtom (const Atom * a) const
{
	if ( (a == Bond::a) )
		return true;

	return false;
}

Atom * Bond::getBondedAtom (const Atom * a) const
{
	if (a == Bond::a)
		return Bond::b;

	if (a == Bond::b)
		return Bond::a;

	return NULL;
}

//
// print Molecule's info
//
void Bond::printInfo (char * name) const
{
	std::ostream * stream = new std::ofstream (name);
	printInfo (stream);
	delete stream;
}

void Bond::printInfo (std::ostream * stream) const
{
	*stream << "Bond between '" <<  a->getName() << "' and '" << b->getName() << "'" << std::endl ;
}

void Bond::printBBStr (std::ostream * stream) const
{
	*stream << "bond " <<  a->getSerial() << " " << b->getSerial() << " " << rmin << " " << rmax << " " << H << std::endl ;
}

