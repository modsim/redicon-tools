/*  testMolecule.cc  2013-07-29 test Molecule class
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

#include "molecule.h"
#include "monatomic.h"


bool PrintAtom (const Atom & a, void * data)
{
	double x = *((double*) data);
	a.printInfo (&std::cout);
	
	if (x < a.getHSRadius())
		return false;
	else
		return true;
}

int main (int argc, char ** argv) 
{
	Atom A ("A", 1.);
	Monatomic M ("M", A);

	Atom A2 ("A2", 1.);
	A2.printInfo (&std::cout);
	Monatomic M2 ("M2", A2);

  	Coord3D P1 (10.,10.,10.);
	M.setPosition (P1);

	// Read PDB
	Molecule * MPDB = NULL;
	try {
		MPDB = new Molecule (FILETYPE_PDB, "1.pdb");
	} catch (const char * msg) {
	    std::cerr << "Error creating a molecule from 1.pdb:  " << msg << std::endl;
	    delete MPDB;
	    MPDB = NULL;
	    std::cerr << "pointer: " << MPDB << std::endl;
	}

	// Add bonds and angles
	MPDB->setBondsLinear (1, 100.);
	MPDB->setAnglesLinear (ANGLE_POTTYPE_SQUARE, 100.);

	if (MPDB)
	{
		MPDB->printBBStr (&std::cout); 
		MPDB->printInfo (&std::cout); 
		MPDB->moveTo(P1);
	}

	// Read PQR
	Molecule * MPQR = NULL;
	try {
		MPQR = new Molecule (FILETYPE_PQR, "dna.pqr");
	} catch (const char * msg) {
		std::cerr << "Error creating a molecule from dna.pqr:  " << msg << std::endl;
		delete MPQR;
		MPQR = NULL;
		std::cerr << "pointer: " << MPQR << std::endl;
	}
	if (MPQR)
	{
		MPQR->printInfo (&std::cout); 
		MPQR->printBBStr (&std::cout); 
		MPQR->moveTo(P1);
	}

	// Test overlap 
	try {
		if (M.overlap (A))
			std::cout << "Atom A overlaps with Molecule M" << std::endl;
	} catch (const char * msg)	
	{
		std::cerr << "Exception thrown: " << msg << std::endl;
	}

	try {
		if (M.overlap (M2))
			std::cout << "Atom A overlaps with Molecule M" << std::endl;
	} catch (const char * msg)	
	{
		std::cerr << "Exception thrown: " << msg << std::endl;
	}

	A.printInfo (&std::cout);
	M.printInfo (&std::cout); 
	if (MPDB)
		MPDB->printInfo (&std::cout); 

	std::cout << std::endl;

	A.printBBStr (&std::cout);
	M.printBBStr (&std::cout);
	if (MPDB)
	{
		MPDB->printBBStr (&std::cout); 

		try {
			double x = 2.;
			MPDB->foreachAtom (PrintAtom, (void*) &x);
		} catch (const char* msg) {
			std::cerr << msg << std::endl;
		}
	}

	if (MPDB)
		delete MPDB;

	if (MPQR)
		delete MPQR;

	return 1;
}

