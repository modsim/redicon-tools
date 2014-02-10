/*  molecule.cc 2013-07-30 valiska@gmail.com
 * 
 *  Copyright (C) 2013 Svyatoslav Kondrat (Valiska)
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stddef.h>
#include <string.h>
#include <iostream> 

#include<molecule.h>

#define _USE_MATH_DEFINES
#include <math.h>

// for C++11 from autoconf
#include <config.h>

//#define DEBUG
#include "defines.h"

Molecule::Molecule (const char * name, Atom a) : owner (NULL)
{
	Molecule::name = strdup (name);
	Atoms.push_back (new Atom(a)); // ignore throw as we check above
	head = Atoms.at (0);

	PositionIsSet = false;

	charge = head->getCharge();
	radius[0] = radius[1] = radius[2] = head->getHSRadius();

	// no need for throw/catch as Atom copy-constructor copies a free atom
	AtomAttorney::claimOwnership (*head, *this);
	AtomAttorney::setSerial (*head, 1);
	AtomAttorney::setType (*head, ATOM_SINGLE);
};

// Read from a file
Molecule::Molecule (unsigned int ft, const char * file) : owner (NULL)
{

	Molecule::name = strdup (file);
	std::ifstream stream (file);
	
	if (stream.rdstate() != std::ios_base::goodbit) 
	{
		free (name);
		throw "Cannot open file." ;
	}

	switch (ft) {
	

		case FILETYPE_PDB:
			if (!readPDB (&stream))
			{
				stream.close();
				free (Molecule::name);
#ifdef HAVE_CXX11
				for (auto &a : Atoms)
					delete a;
#else
				for (unsigned int i = 0; i < getNAtoms(); i++)
					delete Atoms.at (i);
#endif
				throw "Molecule::Molecule(): Reading a PDB file failed";
			}
			break;

		case FILETYPE_PQR: 
			if (!readPQR (&stream))
			{
				free (Molecule::name);
				stream.close();
#ifdef HAVE_CXX11
				for (auto &a : Atoms)
					delete a;
#else
				for (unsigned int i = 0; i < getNAtoms(); i++)
					delete Atoms.at (i);
#endif
				throw "Molecule::Molecule(): Reading a PQR file failed";
			}
			break;

		default:
			free (name);
			stream.close();
			throw "Molecule::Molecule(): Unsupported or unimplemented file type";

	}
	stream.close ();

	head = Atoms.at (0);
	AtomAttorney::setType (*head, ATOM_HEAD);

	DPRINT ("*** File read, running over atoms\n");
	double hmin[3] = {0., 0., 0.};
	double hmax[3] = {0., 0., 0.};
	charge = 0;
#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
#endif
		if (a->positionIsSet())
		{
			Point3D * Ra = a->positionPtr ();
			double radius = a->getHSRadius ();
			for (int i = 0; i < 3; i++)
			{
				double left = Ra->get (i) - radius;
				double right = Ra->get (i) + radius;
				if (left < hmin[i])
					hmin[i] = left;
				if (right > hmax[i])
					hmax[i] = right;
			}
		}
		else
		{
			BCPT_WARNING ("atom's position not set for atom '%s' (serial %li), molecule size might be incorrect",
				a->getName(), a->getSerial());
		}
		charge += a->getCharge();
		AtomAttorney::claimOwnership (*a, *this);
#ifdef DEBUG
		a->printInfo (&std::cerr);
#endif
	}
	for (int i = 0; i < 3; i++)
		radius[i] = 0.5 * (hmax[i] - hmin[i]);
}

// annihilator
Molecule::~Molecule () 
{
	if (owner)
		throw "You cannot destroy me, I have an owner";

	free (name);
#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
		delete a;
	for (auto &b : Bonds) // C++0x
		delete b;
	for (auto &a : Angles) // C++0x
		delete a;
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
		delete Atoms.at (i);
	for (unsigned int i = 0; i < getNBonds(); i++)
		delete Bonds.at (i);
	for (unsigned int i = 0; i < getNAngles(); i++)
		delete Angles.at (i);
#endif // HAVE_CXX11
};

// Position
// set (virtual method, default to check and warn)
bool Molecule::setPosition (const Point3D& R)
{
	if (head->positionPtr() || PositionIsSet) 
	{
		BCPT_WARNING ("Position is set, use Molecule::moveTo() to reset.");
		return false;
	}
		
	
	BCPT_WARNING ("Virtual method, not implemented for a general molecule.");
	return false;
}


bool Molecule::setAtomPosition (Atom * atom, const Point3D & R)
{
	
	BCPT_RET_VAL_IF_FAIL (atom, false);
	Atom * b = atom;  // we are lazy 
	if (!hasAtom (b)) 
	{
		BCPT_ERROR ("Molecule '%s' has not atom '%s'", getName(), b->getName());
		return false;
	}

	if (b->positionIsSet())
	{
		BCPT_ERROR ("Position is set already, use moveTo() [not impl yet]");
		return false;
	}

	// FIXME: make Atom::unsetPosition () and unset when overlaping
	// instead of copying which is expansive
	//Atom bcopy = Atom (*b);
	b->setPosition (R);

#ifdef HAVE_CXX11 
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
#endif
		if(a!=b)
		{
			if (a->positionIsSet())
				if (b->overlap (*a))
				{
					BCPT_ERROR ("Cannot set position: Atom '%s' (serial %li) overlaps with atom '%s' (serial %li)", 
						b->getName(), b->getSerial(), a->getName(), a->getSerial());
					b->unsetPosition();				
					return false;
				}
		}			
	}
	return true;
}

//checks if the molecule has the atom

Atom * Molecule::getAtom (unsigned int s1) const
{
#ifdef HAVE_CXX11
 	for (auto &a : Atoms) 
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i); 
#endif
		if(a->getSerial()==s1)
			return a;
	}
	return NULL;
}

bool Molecule::hasAtom (Atom * b) const
{
 #ifdef HAVE_CXX11 
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
		{
		Atom * a = Atoms.at (i);
#endif
		    if(a==b)
		    { 
			return true;
		    }
		}
	return false;	  
	}


// translate molecule by a vector
bool Molecule::translateBy (const Point3D& T) 
{
	Point3D * po = head->positionPtr();
	if (!po || !PositionIsSet) 
	{
		BCPT_WARNING ("Position not set, cannot translate.");
		return false;
	}

	// if the head position is set, we expect the other atoms' positions to be set
	// so throw an exception if this is not so
#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
		{
		Atom * a = Atoms.at (i);
#endif
		if (!AtomAttorney::translateBy(*a, T))
			throw "Translating atom failed in translateBy()";
		}
	return true;
	}


bool Molecule::moveTo (const Point3D& R) 
{
	Point3D * po = head->positionPtr();

	// if head's position is set, normally the positions of all atoms are set, so just translate
	// we try to translate and throw an eception s we do not expect a 'normal' error
	if (po) 	{	
		Point3D T = po->vectorTo(R);
		try {
			translateBy (T);
		} catch (const char * msg) {
			throw msg;
		}
	}

	return true;
}

// serial number shift
unsigned int Molecule::shiftSerial (unsigned int serial)
{
	// FIXME: this is assuming atoms are arranged in order of their sirial numbers
#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
#endif
		AtomAttorney::setSerial (*a, serial++);
	}

	return serial;
}

double Molecule::getVolume (void) const
{

	double volume = 0.;
	//FIXME: this is untrue when atoms overlap!!!

#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
#endif
		double r = a->getHSRadius ();
		volume += (4. * M_PI / 3. ) * r * r * r;
	}
	return volume;
}


// Overlap 
bool Molecule::overlap (const Atom & ta) const
{

	if (!ta.positionIsSet())
		throw "Molecule::overlap(): Atom's position is not set." ;

	if (!head->positionIsSet())
		throw "Molecule::overlap(): Molecule's position is not set." ;
#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
#endif
		if ( a->overlap(ta) )
			return true;
	}

	return false;
}

bool Molecule::overlap (const Molecule & M) const
{
	if (!M.positionIsSet())
		throw "Molecule::overlap(): Test's molecule position is not set." ;

	if (!head->positionIsSet())
		throw "Molecule::overlap(): Moleculke's position is not set." ;

#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
#endif
		if ( M.overlap(*a) )
			return true;
	}

	return false;
}

// For each loop
bool Molecule::foreachAtom (AtomFunction func, void * data) const
{
#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
	{
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
#endif
		if (!func(*a, data))
			return false;
	}

	return true;
}

//
// print Molecule's info
//
void Molecule::printInfo (char * name) const
{
	std::ostream * stream = new std::ofstream (name);
	printInfo (stream);
	delete stream;
}

void Molecule::printInfo (std::ostream * stream) const
{
	*stream << "Molecule '" << name << "' has " << getNAtoms() << " atom(s), " << getNBonds() << " bond(s). " ;
	*stream << "and " << getNAngles() << " angle bond(s). " ;

	*stream << "Total charge " << getCharge() << ", size (" << radius[0] << ", " << radius[1] << ", " << radius[2] << ")." << std::endl ;
//	*stream << "Total charge " << getCharge() << std::endl ;

}

// Save STR file
void Molecule::printBBStr (const char * name) const
{
	std::ostream * stream = new std::ofstream (name);
	printBBStr (stream);
	delete stream;
}

void Molecule::printBBStr (std::ostream * stream) const
{
#ifdef HAVE_CXX11
	for (auto &a : Atoms) // C++0x
		a->printBBStr (stream);

	for (auto &b : Bonds) // C++0x
		b->printBBStr (stream);

	for (auto &a : Angles) // C++0x
		a->printBBStr (stream);
#else
	for (unsigned int i = 0; i < getNAtoms(); i++)
	{
		Atom * a = Atoms.at (i);
		a->printBBStr (stream);
	}
	for (unsigned int i = 0; i < getNBonds(); i++)
	{
		Bond * b = Bonds.at (i);
		b->printBBStr (stream);
	}
	for (unsigned int i = 0; i < getNAngles(); i++)
	{
		Angle * a = Angles.at (i);
		a->printBBStr (stream);
	}
#endif // HAVE_CXX11


}


