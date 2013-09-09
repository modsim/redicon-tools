/*  angle.h  2013-08-23 Bond Angle between three Atoms
 *  This is part of BCP Tools
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

#if !defined ( __HAVE_ANGLE_H__ )
#  define __HAVE_ANGLE_H__

#include <stdio.h>
#include <iostream> 
#include <fstream>
#include <vector>

#include <atom.h>

typedef enum 
{
	ANGLE_POTTYPE_SQUARE,
	ANGLE_POTTYPE_COSINE
} AnglePotType;


class Angle
{
	public:
		Angle (Atom * a1, Atom * a2, Atom * a3, AnglePotType, double H, double alpha0);
		Angle (Atom * a1, Atom * a2, Atom * a3, AnglePotType, double H); // this set alpha to current angle 180 if none
		Angle (Atom * a1, Atom * a2, Atom * a3, double H); // this set alpha to current angle or 180 if none, and type to square
		Angle (Atom * a1, Atom * a2, Atom * a3, AnglePotType); // this set alpha to current or 180 and H = 0
		Angle (Atom * a1, Atom * a2, Atom * a3); // this set alpha to current or 180 and H = 0 and type to square
		~Angle () {};
		
		//bool isOwned () {if (owner) return true; else return false;};

		// Get info
		double AngleOptimal () const {return alpha0;};
		bool AngleCurrent (double * alpha) const { return calcAngle(alpha);};
		double getH () const {return H;}; // get 'spring' constant'

		bool hasAtom (const Atom * a) const;

		// FIXME: do we really need it?
		bool foreachAtom (AtomFunction, void * user_data) const;

		// print general info
		void printInfo (char*) const;
		void printInfo (std::ostream *) const;
		
		// Output BD_BOX str line
		void printBBStr (std::ostream *) const;

	private:
	
		Atom * a[3];
		AnglePotType type;
		std::string typestr;
		double alpha0, H;

		bool atomsOK (const Atom &a1, const Atom &a2, const Atom &a3) const;
		bool calcAngle (double * angle) const;
		const std::string getTypeStr(void);
};

#endif /* __HAVE_BOND_H__ */
