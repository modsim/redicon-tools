/*  moleculeAngles.cc 2013-08-29 valiska@gmail.com
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

#include<molecule.h>

#define DEBUG
#include "defines.h"


// FIXME: check if the angle bond exists or not
bool Molecule::setAnglesLinear (AnglePotType type, double H)
{
	unsigned long int ia = 0;
	unsigned int na = getNAtoms();

	for (auto &a : Atoms) // C++0x
	{
		unsigned int s = a->getSerial ();
		DPRINT ("Atom '%s' (serial %lu, #%lu/%i):\n", a->getName(), s, ia, na);

		if (ia < na - 2)
		{	
			Atom * a2 = Atoms.at(ia+1);
			DPRINT (" -> next atom '%s' (serial %lu)\n", a2->getName(), a2->getSerial());

			Atom * a3 = Atoms.at(ia+2);
			DPRINT (" -> next to next atom '%s' (serial %lu)\n", a3->getName(), a3->getSerial());

			if ( (a2->getSerial() == s + 1) && (a3->getSerial() == s + 2) )
				Angles.push_back (new Angle (a, a2, a3, type, H));
		}
		ia++;
	}
	return true;
}

