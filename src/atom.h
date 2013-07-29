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

//#include <stdio.h>
#include <iostream> 
#include <fstream>

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
		Atom (char * name, int serial, double radius, double charge);
		Atom (char * string); // get prom PDB HETATM or ATOM string

		~Atom ();

		void setPosition (double X, double Y, double Z) { x=X; y=Y; z=Z;};
		void addBond () {};

		char * getType () const;
		bool setType (AtomType T) {type = T; return true;};

		void print (char*) const;
		void print (std::ofstream *) const;

	private:
		char * name;
		int serial; /* reference number from the PDB file */

		double radius; /* atom is a spherical core */
		double charge;
		AtomType type;

		double x,y,z;

		unsigned int nneighbours; /* atoms connected via bonds (FIXME: do we need it?) */
		Atom ** neighbours;

		unsigned int nbonds; /* pointers to bonds */
		Bonds ** bonds;

		void * userData;
};

#endif /* __HAVE_ATOM_H__ */

