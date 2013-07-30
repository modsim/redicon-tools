/*  molecule.h  2013-07-30 Molecule for BD-CA-PDE tools
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

#if !defined ( __HAVE_MOLECULE_H__ )
#  define __HAVE_MOLECULE_H__

#include <stdio.h>
#include <iostream> 
#include <fstream>

#include <atom.h>

class Molecule
{
	public:
		Molecule (const char * name, Atom &head);
		//Molecule (const char * name, Atom & head, unsigned int nbonds, Bond&, ...) : head (head) {};
		~Molecule ();

		bool addBond (Bond &) {return true;};

		// bool setPosition (double x, double y, double z);
		// bool setPosition (double R[3]) {return true;};
		bool setPosition (Coord3D& R);

		//
		// Get methods
		//
		int getNAtoms () const {return 0;};
		int getNBonds () const {return 0;};

		//
		// Print
		//
		// print general info
		void printInfo (char*) const;
		void printInfo (std::ostream *) const;
		
		// Output BD_BOX str line
		void printBBStr (std::ostream *) const;

	private:

		char * name;

		double radius[3]; // circumferent ellipsoid
		double charge;    // total charge

		Atom & head;

		unsigned int nAtoms; /* atoms connected via bonds (FIXME: do we need it?) */
		Atom ** atoms;

		unsigned int nBonds; /* pointers to bonds */
		Bond ** bonds;

		void * userData;
};

#endif /* __HAVE_MOLECULE_H__ */
