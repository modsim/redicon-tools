/*  moleculecc 2013-07-30 valiska@gmail.com
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

#include "defines.h"

Molecule::Molecule (const char * name, Atom a) : nBonds(0), bonds(NULL), owner (NULL)
{
	Molecule::name = strdup (name);
	Atoms.push_back (new Atom(a)); // ignore throw as we check above
	head = Atoms.at (0);

	charge = head->getCharge();
	radius[0] = radius[1] = radius[2] = head->getHSRadius();

	// no need for throw/catch as Atom copy-constructor copies a free atom
	AtomAttorney::claimOwnership (*head, *this);
	AtomAttorney::setSerial (*head, 1);
	AtomAttorney::setType (*head, ATOM_SINGLE);
};

Molecule::~Molecule () 
{
	free (name);
	for (auto &a : Atoms) // C++0x
		delete a;
	if (bonds) delete [] bonds;
};

// Set position
bool Molecule::setPosition (Point3D& R) 
{
	//for (auto &a : Atoms) // C++0x
		// shift all atoms

	return AtomAttorney::setPosition (*head, R);
}

// serial number shift
unsigned int Molecule::shiftSerial (unsigned int serial)
{
	// FIXME: this is assuming atoms are arranged in order of their sirial numbers
	for (auto &a : Atoms) // C++0x
		AtomAttorney::setSerial (*a, serial++);

	return serial;
}

// Overlap 
bool Molecule::overlap (const Atom & ta) const
{

	if (!ta.positionSet())
		throw "Molecule::overlap(): Atom's position is not set." ;

	if (!head->positionSet())
		throw "Molecule::overlap(): Molecule's position is not set." ;

	for (auto &a : Atoms) // C++0x
		if ( a->overlap(ta) )
			return true;

	return false;
}

bool Molecule::overlap (const Molecule & M) const
{
	if (!M.positionSet())
		throw "Molecule::overlap(): Test's molecule position is not set." ;

	if (!head->positionSet())
		throw "Molecule::overlap(): Moleculke's position is not set." ;

	for (auto &a : Atoms) // C++0x
		if ( M.overlap(*a) )
			return true;

	return false;
}

// For each loop
bool Molecule::foreachAtom (AtomFunction func, void * data) const
{
	for (auto &a : Atoms) // C++0x
		if (!func(*a, data))
			return false;
	return true;
}

//
// print Molecule's info
//
void Molecule::printInfo (char * name) const
{
	std::ostream * stream = new std::ofstream (name);
	printInfo (stream);
	delete stream;
}

void Molecule::printInfo (std::ostream * stream) const
{
	*stream << "Molecule '" << name << "' has " << getNAtoms() << " atom(s) and " << nBonds << " bond(s). ";
//	*stream << "Total charge " << getCharge() << ", size (" << radius[0] << ", " << radius[1] << ", " << radius[2] << ")." << std::endl ;
	*stream << "Total charge " << getCharge() << std::endl ;

}

void Molecule::printBBStr (std::ostream * stream) const
{
	for (auto &a : Atoms) // C++0x
		a->printBBStr (stream);
}

