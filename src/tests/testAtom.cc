/*  testAtom.cc  2013-07-29 test Coord3D class
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

int main (int argc, char ** argv) 
{

	Atom A ("A", 1, 10, 1);
	Atom B ("B", 2, 10, 1);

	Coord3D P1 (0.,0.,0.);
	A.setPosition (P1);

	double x[] = {10., 0.2, 0.7};
	Coord3D * P2 = new Coord3D (x);
	A.setPosition (*P2);

	A.printInfo (&std::cout);
	B.printInfo (&std::cout); 

	std::cout << std::endl;

	A.printBBStr (&std::cout);
	B.printBBStr (&std::cout); 


	delete P2;

	return 1;
}

