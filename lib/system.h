/*  system.h  2013-07-30  System (of Molecules) for BD-CA-PDE tools
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

#if !defined ( __HAVE_SYSTEM_H__ )
#  define __HAVE_SYSTEM_H__

#include <stdio.h>
#include <iostream> 
#include <fstream>

#include <molecule.h>

class System
{
	public:
		System (Coord3D & R0, Coord3D & size); // Box of size size with the center at R0
		~System ();

		bool addMolecule (Molecule &);

		// Get methods
		int getNMolecules () const {return nMolecules;};

		// print general info
		void printInfo (char*) const;
		void printInfo (std::ostream *) const;
		
		// Output BD_BOX str line
		void printBBStr (std::ostream *) const;

	private:

		bool moleculeInBox (Molecule & M);

		Coord3D & R0; // box center
		Coord3D & H;  // box size

		double charge;    // total charge
		unsigned int last_serial; // serial of the last atom (lergest serial)

		unsigned int nMolecules; 
		Molecule ** molecules;
		bool foreachMolecule (MoleculeFunction, void * user_params) const;

		void * userData;
};

#endif /* __HAVE_SYSTEM_H__ */
