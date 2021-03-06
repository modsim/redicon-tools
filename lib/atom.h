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
#include <vector>

#include <point3D.h>
//#include <bond.h>

typedef enum {

  ATOM_FREE,     /* free, not in a molecule (default) */
  ATOM_SINGLE,   /* single as a molecule */
  ATOM_BONDED,   /* bonded, but unknown `structural position' */
  ATOM_HEAD,     /* head group atom: one bond to the tail? */
  ATOM_2BONDS,   /* has two bonds */
  ATOM_BRANCH,   /* more then two bonds */
  ATOM_TERMINAL  /* the last terminal atom: 1 bond */

} AtomType;

typedef enum {
	FILETYPE_PDB,
	FILETYPE_PQR,
	FILETYPE_STR

} FileType;

class Residue;
class Molecule;
class Bond;
class Angle;
class AtomAttorney;

class Atom 
{
	public:

		Atom (const char * name, unsigned int serial, const Point3D & coord,
			double hs_radius, double hd_radius,
			double charge, double LJ, double mass);
		explicit Atom (const char * name, double radius); // simplified constructor
		explicit Atom (unsigned int ft, const std::string & string); // get from a line of a specific file type
		Atom (const Atom &); // copy constructor

		~Atom ();

		// set methods
		void setCharge (double val) { charge = val;};
		void setLJ (double val) {LJ = val;};
		void setMass (double val) { mass = val;};
		void setHDRadius (double val) { hd_radius = val;};

		// Get methods
		const char * getName () const {return name;};
		unsigned int getNNeighbours () const {return Neighbours.size();};
		unsigned int getNBonds () const {return Bonds.size();};
		unsigned int getNAngles () const {return Angles.size();};

		AtomType getType () const {return type;};
		char * getTypeName () const {return typeName;} ;

		unsigned long int getSerial () const {return serial;};

		double getHSRadius () const {return hs_radius;};
		double getHDRadius () const {return hd_radius;};

		double getCharge () const {return charge;};
		double getMass () const {return mass;};

		double getLJ () const {return LJ;};

		// public position stuff (FIXME: private?)
		bool setPosition (const Point3D & R) ;
		bool unsetPosition();
		Point3D * positionPtr () { return r;};
		Point3D * positionCopy () const; // creates Point3D, need delete
		bool positionIsSet () const { if (r) return true; else return false;};

		// checks
		bool overlap (const Atom &) const;
		void overlap_use_HDRadius (void) {use_HDRadius = true;};
		bool overlap_HDRadius (void) const { return use_HDRadius;};

        	bool inUse () const { if (residue || molecule) return true; else return false;};
	
		// print general info
		void printInfo (char*) const;
		void printInfo (std::ostream *) const;
		
		// Output BD_BOX str line
		void printBBStr (std::ostream *) const;

	private:
	
		friend class AtomAttorney; 

		// Read a line of the corresponding file formats
		bool readPDB (const std::string & line);
		bool readPQR (const std::string & line);
		bool readSTR (const std::string & line);

		// Ownerships
		Residue * residue;   // residue I belong to, none=NULL
		bool claimOwnership (Residue & );

		Molecule * molecule; // molecule I belong to, none=NULL;
		bool claimOwnership (Molecule & );

		// Data / parameters
		char * name;
		char * residue_name;
		unsigned int residue_number;

		unsigned long int serial; // reference number from the PDB file
		void setSerial (unsigned long int serial) {Atom::serial = serial;};

		double hs_radius; // atom is a spherical core 
		double charge;

		double LJ;    // LJ parameter (might not be known, default 0.0)
		double hd_radius; // hydrodynamic radius
		double mass;   // might not be known

		bool use_HDRadius;

		// Atom's position (might not ne known or needed)
		Point3D * r; 
		//bool setPosition (const Point3D & R) ;
		bool translateBy (const Point3D & R) ;

		// Atoms type (free, single, bonded, etc)
		AtomType type;
		char * type2name () const;
		char * typeName;
		bool setType (AtomType T);

		std::vector<Atom*> Neighbours;
		std::vector<Bond*> Bonds;
		bool addBond (Bond & b);

		std::vector<Angle*> Angles;
		bool addAngle (Angle & a);

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

		// make position accesebile to a molecule
		static bool setPosition (Atom &a, const Point3D& R) { return a.setPosition(R);};
		static bool translateBy (Atom &a, const Point3D& R) { return a.translateBy(R);};

		static bool addBond (Atom &a, Bond &b) { return a.addBond(b); };
		static bool addAngle (Atom &a, Angle &A) { return a.addAngle(A); };

		friend class Molecule;
		friend class Monatomic;

		friend class Residue;
		friend class Bond;
		friend class Angle;

};

#endif /* __HAVE_ATOM_H__ */

