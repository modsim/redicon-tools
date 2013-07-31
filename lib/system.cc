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

System::System (Coord3D &R0, Coord3D & size) : R0(R0), H(size), charge (0.0), nMolecules(0), molecules (NULL), userData (NULL) { };

System::~System () 
{
	if (molecules) free (molecules);
};


//
// System::moleculeInBox
//
static bool atomInBox (const Atom &a, void * data)
{
	void ** p = (void**) data;
	Coord3D * R0= (Coord3D*) p[0]; // box center
	Coord3D * H = (Coord3D*) p[1]; // box size
	Coord3D * r0 = a.positionCopy (); // atom position

    if (!r0)
        throw "Atom's position not set";

	double  radius = a.getRadius (); 

#ifdef DEBUG
    R0->print (stderr, "Box");
    H->print (stderr, "BosSize");
    r0->print (stderr, "Atom");
#endif

	for (int i = 0; i < 3; i++)
		if ( (r0->getCoord (i) - radius <  R0->getCoord(i) - 0.5 * H->getCoord(i) )
			|| (r0->getCoord (i) + radius >  R0->getCoord(i) + 0.5 * H->getCoord(i) ) )
		{
			delete r0;
            		throw "Out of the box";
		}

    DPRINT ("in the box\n");

	return true;

}
bool System::moleculeInBox (Molecule& M) 
{
	void * p[] = {&R0, &H};

	try {
		M.foreachAtom (atomInBox,(void*) p);
	} catch (const char* msg) {
        BCPT_ERROR ("Molecule not in box (reason: %s).", msg);
		return false;
	}

	return true;
}

//
// Add a molecule
//
bool System::addMolecule (Molecule& M) 
{
    
    for (unsigned int i = 0; i < nMolecules; i++)
        if (molecules[i] == &M)
        {
            BCPT_WARNING ("Molecule already added, skipping");
            return false;
        }
    // FIXME: check if molecules intersect

	if (moleculeInBox (M))
	{
		nMolecules++;
		molecules = (Molecule**) realloc (molecules, nMolecules * sizeof(Molecule));
		molecules[nMolecules-1] = &M;
        charge += M.getCharge();
	}
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
	*stream << "System has " << nMolecules << " molecules in a box of size ";
	H.print (stream);
	*stream << " located at "; R0.print (stream);
	*stream  << "and its total charge is " << charge << std::endl ;
}

void System::printBBStr (std::ostream * stream) const
{
	for (unsigned int i = 0; i < nMolecules; i++)
		(molecules[i])->printBBStr (stream);
}

