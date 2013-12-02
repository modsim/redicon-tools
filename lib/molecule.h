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
#include <vector>

#include <atom.h>
#include <bond.h>
#include <angle.h>

class System;
class MoleculeAttorney;

class Molecule
{
	public:

		// NOTE: molecules can get more then one head, we consider this as association
		Molecule (const char * name, Atom head);
		Molecule (unsigned int ft, const char * file); 
		~Molecule ();

		bool isOwned () {if (owner) return true; else return false;};
		
		// add an atom and create a bond. The serial is incremented from 1 (head atom)
		bool addAtom (Atom & a, unsigned int serial, double blength, double eps, double H);
		
		// create a bond betwen two Atoms
		bool createBond (Atom * a, Atom * b, double blength, double eps, double H);
		// create a bond between two atoms with serial s1 and s2
		bool createBond (unsigned int s1, unsigned int s2, double blength, double eps, double H);

		// Bind atoms with their neighbours (determined by the serial numbers)
		// if they are not yet binded
		bool setBondsLinear (double eps, double H);
		// set current angles to optimal lineraly along the molecule
		bool setAnglesLinear (AnglePotType type, double H);

		// Position
		bool positionIsSet () const {return PositionIsSet;};
		Point3D * positionCopy () const { return head->positionCopy();};
 		Point3D * positionPtr () { return head->positionPtr();};
                
		//if the molecule has atom
		bool hasAtom(Atom * a) const;
		
                Atom * getAtom(unsigned int s1) const;
		// this is initial position set, depends on the molecule type (polymer, one atom
		virtual bool setPosition (const Point3D & R);	
		// move or translate a molecule
		bool moveTo (const Point3D & R) ;
		bool translateBy (const Point3D & T);

		// Get info
		unsigned int getNAtoms () const {return Atoms.size();};
		unsigned int getNBonds () const {return Bonds.size();};
		unsigned int getNAngles () const {return Angles.size();};

		double getCharge () const {return charge;};
		char * getName () const {return name;};
		double getVolume () const;
		Point3D getSize () const { return Point3D(radius[0], radius[1], radius[2]);};

		// checks for overlaps
		// throws an exception if atom/molecule's position not set
		bool overlap (const Atom &) const;
		bool overlap (const Molecule &) const;

		// FIXME: do we really need it?
		bool foreachAtom (AtomFunction, void * user_data) const;

		// print general info
		void printInfo (char*) const;
		void printInfo (std::ostream *) const;
		
		// Output BD_BOX str line
		void printBBStr (const char *) const;
		void printBBStr (std::ostream *) const;

	protected:
	
		Atom * head;
		std::vector<Atom*> Atoms;
		std::vector<Bond*> Bonds;
		std::vector<Angle*> Angles;

		bool PositionIsSet;

		// TODO:
		//addAtom (Atom &);
		//bool createBond (i, j) {return true;};
		// ...

	private:

		friend class MoleculeAttorney;
		
		bool readPDB (std::ifstream *);
		bool readPQR (std::ifstream *);

		// TODO:
		//addAtom (Atom &);
		//getAtom (unsigned int );
		//addBond (unsigned int, unsigned int);
		//addAngle (unsigned int, unsigned int, unsigned int);
		//addDihedral (unsigned int, unsigned int, unsigned int, unsigned int);
		//checkComplete () ; // check if all atoms involved in bonds etc

		unsigned int serial; // first atom's serial
		unsigned int shiftSerial (unsigned int); // shift because it always set first atom to 1. Returns the serial of the last atom
		unsigned int getSerial () const { return serial;}; // FIXME: why not make it public?

		char * name;

		double radius[3]; // circumferent ellipsoid
		double charge;    // total charge
		
		System * owner;
		bool claimOwnership (System & s) { if (!owner) {owner = &s; return true;} else return false;};
		bool releaseOwnership (System & s) { if (owner == &s) {owner = NULL; return true;} else return false;};

		void * userData;
};

typedef bool (MoleculeFunction) (const Molecule &, void * user_data);

class MoleculeAttorney
{
	private:
		static unsigned int shiftSerial (Molecule &M, unsigned int serial) { return M.shiftSerial(serial); };
		static unsigned int getSerial (Molecule &M) { return M.getSerial(); };
		static bool claimOwnership (Molecule & M, System & s) {return M.claimOwnership (s);};
		static bool releaseOwnership (Molecule & M, System & s) {return M.releaseOwnership (s);};

		friend class System;
};

#endif /* __HAVE_MOLECULE_H__ */
