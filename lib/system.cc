/*  system.cc 2013-07-30 valiska@gmail.com
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

#include"system.h"

//#define DEBUG
#include "defines.h"

System::System (Coord3D &R0, Coord3D & size) : R0(R0), H(size), charge (0.0), last_serial (1), trydelete (false), userData (NULL) { };

System::~System () 
{
	for (auto &m : Molecules) // C++0x
	{
		MoleculeAttorney::releaseOwnership (*m, *this);
		if (trydelete)
		{
			try {
				delete m;
			} catch (const char * msg) {
				BCPT_WARNING ("Deleting molecule '%s' failed: %s", m->getName(), msg);
			}
		}
	}
};

// For System::moleculeInBox
static bool atomInBox (const Atom &a, void * data)
{
	void ** p = (void**) data;
	Coord3D * R0= (Coord3D*) p[0]; // box center
	Coord3D * H = (Coord3D*) p[1]; // box size
	Coord3D * r0 = a.positionCopy (); // atom position

	if (!r0)
	{
		delete r0;
		throw "Atom's position not set";
	}

	double  radius = a.getHSRadius (); 

#ifdef DEBUG
	R0->print (stderr, "Box");
	H->print (stderr, "BosSize");
	r0->print (stderr, "Atom");
#endif

	for (int i = 0; i < 3; i++)
		if ( (r0->get (i) - radius <  R0->get(i) - 0.5 * H->get(i) )
				|| (r0->get (i) + radius >  R0->get(i) + 0.5 * H->get(i) ) )
		{
			delete r0;
			return false;
		}

	delete r0;
	return true;
}

// Private method 
bool System::moleculeInBox (Molecule& M) 
{
	void * p[] = {&R0, &H};
	bool res = false;

	try {
		res = M.foreachAtom (atomInBox,(void*) p);
	} catch (const char* msg) {
		BCPT_ERROR ("Molecule '%s' not in box (reason: %s).", M.getName(), msg);
		return false;
	}
	return res;
}

// For foreachMolecule used in System::addMolecule
static bool moleculeNoOverlap (const Molecule & m, void * p)
{
	Molecule * M = (Molecule*) p;
	bool overlap = M->overlap(m);

	if (overlap)
		return false;
	
	return true;
}

// Add a molecule
bool System::addMolecule (Molecule& M) 
{

	if (!M.positionIsSet())
	{
		BCPT_ERROR ("Molecule '%s' position is not set, cannot add.", M.getName());
		return false;
	}

	for (auto &m : Molecules)
		if (m == &M)
		{
			BCPT_ERROR ("Molecule '%s' already added, skipping", M.getName());
			return false;
		}

	// if M in box
	if (!moleculeInBox (M))
	{
		BCPT_ERROR ("Molecule '%s' is outside system's box", M.getName());
		return false;
	}

	// check if M intersects with the molecules
	try {
		bool nooverlap = foreachMolecule (moleculeNoOverlap, (void*) &M);
		if (!nooverlap)
		{
			BCPT_ERROR ("Molecule '%s' overlaps with other  molecules in the system", M.getName());
			return false;
		}
	} catch (const char * msg) {
		BCPT_ERROR ("Exception thrown: %s", msg);
		return false;
	}

	if (MoleculeAttorney::claimOwnership (M, *this))
	{
		Molecules.push_back (&M);
		last_serial = MoleculeAttorney::shiftSerial (M, last_serial);
		charge += M.getCharge();
		return true;
	}
	
	BCPT_ERROR ("cannot add a molecule to the system, the molecules is owned by someone else.");
	return false;
}

// For each loop
bool System::foreachMolecule (MoleculeFunction func, void * data) const
{
	for (auto &m : Molecules)
		if (!func(*m, data))
			return false;
	return true;
}

//
// print System's info
//
void System::printInfo (char * name) const
{
	std::ostream * stream = new std::ofstream (name);
	printInfo (stream);
	delete stream;
}

void System::printInfo (std::ostream * stream) const
{
	*stream << "System has " << getNMolecules() << " molecules in a box of size ";
	H.print (stream);
	*stream << " centered at "; R0.print (stream);
	*stream  << "and its total charge is " << charge << std::endl ;
}

void System::printBBStr (std::ostream * stream) const
{
	for (auto &m : Molecules)
		m->printBBStr (stream);
}

