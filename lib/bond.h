/*  bond.h  2013-08-05 Atomic Bond for Molecule of BD-CA-PDE tools
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

#if !defined ( __HAVE_BOND_H__ )
#  define __HAVE_BOND_H__

#include <stdio.h>
#include <iostream> 
#include <fstream>
#include <vector>

#include <atom.h>

class Bond
{
	public:
		Bond (Atom * a1, Atom * a2);
		Bond (Atom * a1, Atom * a2, double rmin, double rmax, double H);
		~Bond () {};
		
		//bool isOwned () {if (owner) return true; else return false;};

		// Get info
		double getRmin () const {return rmin;};
		double getRmax () const {return rmax;};
		double getLength () const;

		bool hasAtom (const Atom * a) const;
		Atom * getBondedAtom (const Atom * a) const; // get a bonded atom or NULL if a does not belong to the bond

		// FIXME: do we really need it?
		bool foreachAtom (AtomFunction, void * user_data) const;

		// print general info
		void printInfo (char*) const;
		void printInfo (std::ostream *) const;
		
		// Output BD_BOX str line
		void printBBStr (std::ostream *) const;

	private:
	
		Atom * a;
		Atom * b;
		double rmin, rmax, H;

		void * userData;
};

#endif /* __HAVE_BOND_H__ */
