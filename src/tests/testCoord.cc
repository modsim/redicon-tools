/*  testCoord.cc  2013-07-29 test Coord3D class
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

#include "coord3D.h"

int main (int argc, char ** argv) 
{

	Coord3D P1 (2, .1, .2);
	P1.setCoord (0, 10.);

	Coord3D P3 = P1;
	P3.setCoord (0, 5.);

	double x[] = {0., 0., 0.};
	Coord3D * P2 = new Coord3D (x);

	P1.print (stdout, "P1"); std::cout << std::endl;
	P2->print (stdout, "P2"); std::cout << std::endl;
	P3.print (stdout, "P3"); std::cout << std::endl;

	std::cout << "|P1,P2| = " << P1.distance (*P2) << std::endl ;
	std::cout << "|P2,P1| = " << P2->distance (P1) << std::endl ;
	std::cout << std::endl;

	std::cout << "|P1,P3| = " << P1.distance (P3) << std::endl ;
	std::cout << "|P3,P1| = " << P3.distance (P1) << std::endl ;

	delete P2;

	return 1;
}

