/*  atom.h  2013-07-29 Atom for BD-CA-PDE tools
 *
 * Copyright (C) 2007 Svyatoslav Kondrat (Valiska)
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

#if !defined ( __HAVE_ATOM_H__ )
#  define __HAVE_ATOM_H__

#include <stdio.h>
#include <iostream> 
#include <fstream>

#include <coord3D.h>

typedef enum {

  ATOM_FREE,     /* default */
  ATOM_BONDED,   /* bonded, but unknown `structural position' */
  ATOM_HEAD,     /* head group atom: one bond to the tail? */
  ATOM_2BONDS,   /* has two bonds */
  ATOM_BRANCH,   /* more then two bonds */
  ATOM_TERMINAL  /* the last terminal atom: 1 bond */

} AtomType;

class Bonds;

class Atom 
{
	public:
		Atom (const char * name, int serial, double radius, double charge);
		Atom (const char * string) {}; // get prom PDB HETATM or ATOM string
		~Atom ();

		// FIXME: friend/Attorney for Molecule/Residue/Bond ?
		void setPosition (double x, double y, double z) { r = new Coord3D (x,y,z); };
		void setPosition (double R[3]) { r = new Coord3D (R[0],R[1],R[2]); };
		void setPosition (Coord3D& R) { r = new Coord3D (R);};

		void addBond () {};

		char * getType () const;

		// FIXME: friend/Attorney for Molecule/Residue/Bond ?
		bool setType (AtomType T) {type = T; return true;};
		
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
		int serial; /* reference number from the PDB file */

		double radius; /* atom is a spherical core */
		double charge;
		Coord3D * r; // position might not ne known or needed (make 0,0,0 ?)

		AtomType type;

		unsigned int nneighbours; /* atoms connected via bonds (FIXME: do we need it?) */
		Atom ** neighbours;

		unsigned int nbonds; /* pointers to bonds */
		Bonds ** bonds;

		void * userData;
};

#endif /* __HAVE_ATOM_H__ */

