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

  ATOM_FREE,     /* free, not in a molecule (default) */
  ATOM_SINGLE,   /* single as a molecule */
  ATOM_BONDED,   /* bonded, but unknown `structural position' */
  ATOM_HEAD,     /* head group atom: one bond to the tail? */
  ATOM_2BONDS,   /* has two bonds */
  ATOM_BRANCH,   /* more then two bonds */
  ATOM_TERMINAL  /* the last terminal atom: 1 bond */

} AtomType;

class Bond;
class Residue;
class Molecule;

class AtomAttorney;

class Atom 
{
	public:

		Atom (const char * name, unsigned int serial, const Coord3D & coord,
			double hs_radius, double hd_radius,
			double charge, double LJ, double mass);
		Atom (const char * name, double radius); // simplified constructor
		Atom (const Atom &); // copy constructor
		Atom (const char * string) {}; // get prom PDB HETATM or ATOM string, or from BD_BOX's STR
		~Atom ();

		// Get methods
		char * getName () const {return name;};

		AtomType getType () const {return type;};
		char * getTypeName () const {return typeName;} ;

		int getSerial () const {return serial;};

		double getHSRadius () const {return hs_radius;};
		double getHDRadius () const {return hd_radius;};

		double getCharge () const {return charge;};
		double getMass () const {return mass;};

		double getLJ () const {return LJ;};

		// public position stuff (FIXME: private?)
		Coord3D * positionPtr () { return r;};
		Coord3D * positionCopy () const; // creates Coord3D, need delete
		bool positionSet () const { if (r) return true; else return false;};

		// checks
		bool overlap (const Atom &) const;
        	bool inUse () const { if (residue || molecule) return true; else return false;};
	
		// print general info
		void printInfo (char*) const;
		void printInfo (std::ostream *) const;
		
		// Output BD_BOX str line
		void printBBStr (std::ostream *) const;

	private:
	
		friend class AtomAttorney; 

		// Ownerships
		Residue * residue;   // residue I belong to, none=NULL
		bool claimOwnership (Residue & );

		Molecule * molecule; // molecule I belong to, none=NULL;
		bool claimOwnership (Molecule & );

		// Data / parameters
		char * name;
		int serial; // reference number from the PDB file
		void setSerial (unsigned int serial) {Atom::serial = serial;};

		double hs_radius; // atom is a spherical core 
		double charge;

		double LJ;    // LJ parameter (might not be known, default 0.0)
		double hd_radius; // hydrodynamic radius
		double mass;   // might not be known

		// Atom's position (might not ne known or needed)
		Coord3D * r; 
		bool setPosition (Coord3D& R) ;

		// Atoms type (free, single, bonded, etc)
		AtomType type;
		char * type2name () const;
		char * typeName;
		bool setType (AtomType T);

		unsigned int nneighbours; /* atoms connected via bonds (FIXME: do we need it?) */
		Atom ** neighbours;

		unsigned int nbonds; /* pointers to bonds */
		Bond ** bonds;
		bool addBond (Bond &b) { return true;};

		void * userData;
};

typedef bool (AtomFunction) (const Atom &, void * user_data);

// FIXME: separate Attorney for Molecules, Residues and Bonds
class AtomAttorney
{
	private:

		static bool claimOwnership (Atom &a, Residue &r) { return a.claimOwnership(r);};
		static bool claimOwnership (Atom &a, Molecule &m) { return a.claimOwnership(m);};

		static void setSerial (Atom &a, unsigned int serial) { return a.setSerial (serial);};
		static bool setType (Atom &a, AtomType t) { return a.setType(t); };

		static bool setPosition (Atom &a, Coord3D& R) { return a.setPosition(R);};
		static bool addBond (Atom &a, Bond &b) { return a.addBond(b); };

		friend class Molecule;
		friend class Residue;
		friend class Bond;
};

#endif /* __HAVE_ATOM_H__ */

