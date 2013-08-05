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
	if (owner)
		throw "You cannot destroy me, I have an owner";

	free (name);
	for (auto &a : Atoms) // C++0x
		delete a;
	if (bonds) delete [] bonds;
};

// Position
// set (virtual method, default to check and warn)
bool Molecule::setPosition (const Point3D& R)
{
	if (head->positionPtr()) 
	{
		BCPT_WARNING ("Position is set, use Molecule::moveTo() to reset.");
		return false;
	}

	BCPT_WARNING ("Virtual method, not implemented for a general molecule.");
	return false;
}

// translate molecule by a vector
bool Molecule::translateBy (const Point3D& T) 
{
	Point3D * po = head->positionPtr();
	if (!po) 
	{
		BCPT_WARNING ("Position not set, cannot translate.");
		return false;
	}

	// if the head position is set, we expect athe other atoms' positions to be set
	// so throw an exception if this is not so
	for (auto &a : Atoms) // C++0x
		if (!AtomAttorney::translateBy(*a, T))
			throw "Translating atom failed in translateBy()";

	return true;
}
bool Molecule::moveTo (const Point3D& R) 
{
	Point3D * po = head->positionPtr();

	// if head's position is set, normally the positions of all atoms are set, so just translate
	// we try to translate and throw an eception s we do not expect a 'normal' error
	if (po) 	{	
		Point3D T = po->vectorTo(R);
		try {
			translateBy (T);
		} catch (const char * msg) {
			throw msg;
		}
	}

	return true;
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

	if (!ta.positionIsSet())
		throw "Molecule::overlap(): Atom's position is not set." ;

	if (!head->positionIsSet())
		throw "Molecule::overlap(): Molecule's position is not set." ;

	for (auto &a : Atoms) // C++0x
		if ( a->overlap(ta) )
			return true;

	return false;
}

bool Molecule::overlap (const Molecule & M) const
{
	if (!M.positionIsSet())
		throw "Molecule::overlap(): Test's molecule position is not set." ;

	if (!head->positionIsSet())
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

