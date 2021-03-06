/*  testSystem.cc  2013-07-29 test System class
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

#include "system.h"

int main (int argc, char ** argv) 
{
	Atom A ("A", 1);
	Molecule M ("M", A);
	Molecule M2 ("M2", A);

	Coord3D P1 (1.,1.,1.);
	M.setPosition (P1);

	Coord3D P2 (1.,2,1.);
	M2.setPosition (P2);

	Coord3D P3 (2.,2,1.);
	Molecule M3 ("M3", A);
	M3.setPosition (P3);

	Point3D X0 (0.,0.,0.);
	Coord3D size (6., 6., 6.); // Point3D and Coord3D are synonyms

	System * S0 = new System (X0, size);
	S0->addMolecule (M);
	S0->addMolecule (M2);
	S0->printInfo (&std::cout);
	S0->printBBStr (&std::cout);
	delete S0;

	System S (X0, size);

	S.addMolecule (M);
	S.addMolecule (M2);
	S.addMolecule (M3);
	S.addMolecule (M3);

	S.printInfo (&std::cout);
	S.printBBStr (&std::cout);

	return 1;
}

