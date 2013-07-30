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

	Atom A ("A", 1, 1, 1);
	Molecule M ("M", A);

	Atom A2 ("A2", 1, 1, 1);
	Molecule M2 ("M2", A2);

  	Coord3D P1 (1.,1.,1.);
	M.setPosition (P1);
	//M2.setPosition (P1);

    Coord3D X0 (0.,0.,0.);
    Coord3D size (4., 4., 4.);
    System S (X0, size);

    S.addMolecule (M);
    S.addMolecule (M2);

    S.printInfo (&std::cout);
    S.printBBStr (&std::cout);

	return 1;
}

