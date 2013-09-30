/*  moleculeBonds.cc 2013-08-27 valiska@gmail.com
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

//#define DEBUG
#include "defines.h"

bool Molecule::setBondsLinear (double eps, double H)
{
	unsigned long int ia = 0;
#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
	{
#else 
	for (int i = 0; i< getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
#endif
		DPRINT ("Atom '%s' (serial %lu, #%lu) is not bonded\n", a->getName(), a->getSerial(), ia);

		if ( a->getNBonds() == 0 )
		{ 
			// unbonded atom, bond to the neighbouring atoms from serials
			unsigned int s = a->getSerial ();
			DPRINT ("Atom '%s' (serial %lu, #%lu) is not bonded\n", a->getName(), s, ia);
			if (getNAtoms() == 1)
				AtomAttorney::setType (*a, ATOM_SINGLE);

			else if ( (ia == 0) && (getNAtoms() > 1) )
			{	
				// connect only if molecules in series
				DPRINT ("\tit is a head atom\n");
				Atom * next = Atoms.at(ia+1);
				if (next->getSerial() == s + 1)
				{
					Bonds.push_back (new Bond (a, next, eps, H));
					AtomAttorney::setType (*a, ATOM_HEAD);
				}
			}

			else if ( (ia == getNAtoms() - 1) && (getNAtoms() > 1) )
			{
				DPRINT ("\tit is a terminal atom\n");
				Atom * prev = Atoms.at(ia-1);
				if (prev->getSerial() == s-1)
				{
					Bonds.push_back (new Bond (a, prev, eps, H));
					AtomAttorney::setType (*a, ATOM_TERMINAL);
				}
			}

			else
			{
				DPRINT ("\tit is a bonded atom\n");
				Atom * next = Atoms.at(ia+1);
				if (next->getSerial() == s + 1)
				{
					Bonds.push_back (new Bond (a, next, eps, H));
					AtomAttorney::setType (*a, ATOM_BONDED);
				}
				Atom * prev = Atoms.at(ia-1);
				if (prev->getSerial() == s-1)
				{
					Bonds.push_back (new Bond (a, prev, eps, H));
					AtomAttorney::setType (*a, ATOM_BONDED);
				}
			}
		}
		ia++;
	}

	return true;
}


