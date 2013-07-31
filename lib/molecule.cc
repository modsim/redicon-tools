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

Molecule::Molecule (const char * name, Atom a) : head(a), nBonds(0), bonds(NULL)
{
	Molecule::name = strdup (name);
	nAtoms = 1;
	atoms = new Atom* [nAtoms];
	atoms[0] = &head;

	charge = atoms[0]->getCharge();
	radius[0] = radius[1] = radius[2] = atoms[0]->getRadius();
    AtomAttorney::block(head);
};

Molecule::~Molecule () 
{
	free (name);
    for (unsigned int i = 0; i < nAtoms; i++)
        AtomAttorney::unblock(*atoms[i]);
	if (nAtoms) delete [] atoms;
	if (bonds) delete [] bonds;
};

// Set position
bool Molecule::setPosition (Coord3D& R) 
{
	return AtomAttorney::setPosition (head, R);
}

// Overlap 
bool Molecule::overlap (const Atom & a) const
{

	if (!a.positionSet())
		throw "Molecule::overlap(): Atom's position is not set." ;

	if (!head.positionSet())
		throw "Molecule::overlap(): Molecule's position is not set." ;

	for (unsigned int i = 0; i < nAtoms; i++)
		if ( atoms[i]->overlap(a) )
			return true;

	return false;
}

bool Molecule::overlap (const Molecule & M) const
{
	if (!M.positionSet())
		throw "Molecule::overlap(): Test's molecule position is not set." ;

	if (!head.positionSet())
		throw "Molecule::overlap(): Moleculke's position is not set." ;

	for (unsigned int i = 0; i < nAtoms; i++)
		if ( M.overlap(*atoms[i]) )
			return true;

	return false;
}

// For each loop
bool Molecule::foreachAtom (AtomFunction func, void * data) const
{
	for (unsigned int i = 0; i < nAtoms; i++)
		if (!func(*(atoms[i]), data))
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
	*stream << "Molecule '" << name << "' has " << nAtoms << " atom(s) and " << nBonds << " bond(s).";
	*stream << "Total charge " << charge << ", size (" << radius[0] << ", " << radius[1] << ", " << radius[2] << ")." << std::endl ;
}

void Molecule::printBBStr (std::ostream * stream) const
{

	for (unsigned int i = 0; i < nAtoms; i++)
		(atoms[i])->printBBStr (stream);
}

