/*  molecule.cc 2013-07-30 valiska@gmail.com
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

// for C++11 from autoconf
#include <config.h>

//#define DEBUG
#include "defines.h"

// Create a bond when adding an atom, as an atom w/o bond in a molecule makes no sense
// serial is the serial of an atom to bond with -- FIXME: add a comment in molecule.h
bool Molecule::addAtom (Atom & a, unsigned int s, double blength, double eps, double H)
{	
	// Serial number of the head
	unsigned int shead = Atoms.at(0)->getSerial();
	unsigned int s1 = shead; // serial `counter` -- to be the new/added atom serial

	bool hasSerial = false;

	// Get next serial to assign to a new atom 
	// in principle it is just NAtom(), but run a loop and check if atoms are in series
#ifdef HAVE_CXX11
 	for (auto &b : Atoms) 
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * b = Atoms.at (i); 
#endif		
		DPRINT ("shead=%i, s1=%i, s0=%i (natoms=%i)\n", shead, s1, b->getSerial(), getNAtoms());

		if (b->getSerial() == s)
		    hasSerial = true;
		
		// skip the first/head atom
		if (b->getSerial() == shead)
			continue;

		if (b->getSerial() == s1 + 1)
		{
			s1 = b->getSerial();
		}
		else
			throw "Molecule::addAtom(): Serial numbers not in series as expected";
	}
	// the serial of the new/added atom
	s1++;

	if (! hasSerial)
	{
		BCPT_ERROR ("cannot add an atom: no atom with serial %i in the molecule to creat a bond", s);
		return false;
	}
	
	// now add an atom to the list, and reset the charge and radius
	Atom * A = new Atom (a);
 	Atoms.push_back (A);
    	charge += A->getCharge();
	// set radius to zero as positions are not known yet
	radius[0] = radius[1] = radius[2] = 0.0;

    	// Set atom properties
    	AtomAttorney::claimOwnership (*A, *this);
    	AtomAttorney::setSerial (*A, s1);
	AtomAttorney::setType (*A, ATOM_TERMINAL);

	// Change bonded atom status
	Atom * B = getAtom (s);
	if ( strcmp (B->getTypeName(), "ATOM_TERMINAL") && s != shead )
	    	AtomAttorney::setType (*B, ATOM_2BONDS);
	else if ( strcmp (B->getTypeName(), "ATOM_2BONDS") && s != shead )
		AtomAttorney::setType (*B, ATOM_BRANCH);

	//finally create a bond
	return createBond (s, s1, blength, eps, H);
};

bool Molecule::createBond (Atom * a, Atom * b, double blength, double eps, double H)
{

	BCPT_RET_VAL_IF_FAIL (a, false);
	BCPT_RET_VAL_IF_FAIL (b, false);
	BCPT_RET_VAL_IF_FAIL (a != b,false);

	if (hasAtom(a) && hasAtom (b))
		Bonds.push_back (new Bond (a, b, blength, eps, H));
	else
	{
		BCPT_ERROR ("atoms not in a molecule, cannot create a bond");
		return false; 
	}
	return true;
};

bool Molecule::createBond (unsigned int s1, unsigned int s2, double blength, double eps, double H)
{

	Atom * a1 = getAtom(s1);
	Atom * a2 = getAtom(s2);
  
  	return createBond (a1, a2, blength, eps, H);
};
