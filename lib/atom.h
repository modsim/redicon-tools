/*  atom.h  2013-07-29 Atom for BD-CA-PDE tools
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

class Bond;
class Molecule;
class AtomAttorney;

class Atom 
{
	public:
		Atom (const char * name, int serial, double radius, double charge);
		Atom (const char * string) {}; // get prom PDB HETATM or ATOM string
		~Atom ();

		//
		// Get methods
		//
		AtomType getType () const {return type;};
		char * getTypeName () const {return typeName;} ;

		char * getName () const {return name;};
		int getSerial () const {return serial;};
		double getRadius () const {return radius;};
		double getCharge () const {return charge;};

		Coord3D * positionPtr () { return r;};
		Coord3D * positionCopy () const; // creates Coord3D

		// checks
		bool overlap (const Atom &) const;
        	bool inUse () const { if (owned) return true; else return false;};
	
		//
		// Print
		//
		// print general info
		void printInfo (char*) const;
		void printInfo (std::ostream *) const;
		
		// Output BD_BOX str line
		void printBBStr (std::ostream *) const;

	private:

		bool addBond (Bond &b) { return true;};
		bool setType (AtomType T);

		void setPosition (double x, double y, double z) { r = new Coord3D (x,y,z);};
		void setPosition (double R[3]) { r = new Coord3D (R[0],R[1],R[2]); };
		void setPosition (Coord3D& R) { r = new Coord3D (R);};

        unsigned int owned;
        void block () { if (owned == 0) owned++;};
        void unblock () { if (owned != 0) owned--;};

		friend class AtomAttorney; 
	
        // Data
		char * name;
		int serial; /* reference number from the PDB file */

		double radius; /* atom is a spherical core */
		double charge;
		Coord3D * r; // position might not ne known or needed (make 0,0,0 ?)

		AtomType type;
		char * type2name () const;
		char * typeName;

		unsigned int nneighbours; /* atoms connected via bonds (FIXME: do we need it?) */
		Atom ** neighbours;

		unsigned int nbonds; /* pointers to bonds */
		Bond ** bonds;

		void * userData;
};

typedef bool (AtomFunction) (const Atom &, void * user_data);

// Fixme Different Attorney for different classes
class AtomAttorney
{
	private:
        
        // this for molecules only -> separate Attorney into Mol and Bond
        static void block (Atom &a) {a.block();};
        static void unblock (Atom &a) {a.unblock();};

		static bool addBond (Atom &a, Bond &b) { return a.addBond(b); };
		static bool setType (Atom &a, AtomType t) { return a.setType(t); };
		static void setPosition (Atom &a, double x, double y, double z) { a.r = new Coord3D (x,y,z); };
		static void setPosition (Atom &a, double R[3]) { a.r = new Coord3D (R[0],R[1],R[2]); };
		static void setPosition (Atom &a, Coord3D& R) { a.r = new Coord3D (R);};
		//static Coord3D * getPosition (const Atom &a)  { return a.getPosition(); };

		friend class Molecule;
		friend class Bond;
};

#endif /* __HAVE_ATOM_H__ */
