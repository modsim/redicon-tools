/*  testAtom.cc  2013-07-29 test Atom class
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

#include "atom.h"
#include "bond.h"

int main (int argc, char ** argv) 
{

	Atom A ("A", 1.);
	Atom B ("B", 2.);

// Read a PDB line
#define atom_pdb "ATOM    145  N   VAL A  25      32.433  16.336  57.540  1.00 11.92      A1   N"
	try {
		Atom C (FILETYPE_PDB, atom_pdb);
		C.printInfo (&std::cout); 
		C.printBBStr (&std::cout); 
	} catch (const char * msg) {
		std::cerr << msg << std::endl;
	}

// Read a PQR line
#define atom_pqr "ATOM      1  N   MET     1      45.673 -25.260  52.998 -0.3200 2.0000"
	try {
		Atom C (FILETYPE_PQR, atom_pqr);
		C.printInfo (&std::cout); 
		C.printBBStr (&std::cout); 
	} catch (const char * msg) {
		std::cerr << msg << std::endl;
	}

	// Test overlap
	try {
		A.overlap (B);
	} catch (const char * msg) {
		std::cerr << "error checking overlap: " << msg << std::endl;
	}

	// Test bonds
	Bond b (&A, &B);
	b.printInfo(&std::cout);
	b.printBBStr(&std::cout); 

	A.printInfo (&std::cout);
	B.printInfo (&std::cout); 

	std::cout << std::endl;

	A.printBBStr (&std::cout);
	B.printBBStr (&std::cout); 

	return 1;
}

