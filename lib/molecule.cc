/*  molecule.cc 2013-07-30 valiska@gmail.com
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

#include <stddef.h>
#include <string.h>
#include <iostream> 

#include<molecule.h>

//#define DEBUG
#include "defines.h"

Molecule::Molecule (const char * name, Atom a) : owner (NULL)
{
	Molecule::name = strdup (name);
	Atoms.push_back (new Atom(a)); // ignore throw as we check above
	head = Atoms.at (0);

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
				for (auto &a : Atoms)
					delete a;
				throw "Molecule::Molecule(): Reading a PDB file failed";
			}
			break;

		case FILETYPE_PQR: 
			if (!readPQR (&stream))
			{
				free (Molecule::name);
				stream.close();
				for (auto &a : Atoms)
					delete a;
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
	charge = 0;
	for (auto &a : Atoms) // C++0x
	{
		charge += a->getCharge();
		AtomAttorney::claimOwnership (*a, *this);
#ifdef DEBUG
		a->printInfo (&std::cerr);
#endif
	}
}

// annihilator
Molecule::~Molecule () 
{
	if (owner)
		throw "You cannot destroy me, I have an owner";

	free (name);
	for (auto &a : Atoms) // C++0x
		delete a;
	for (auto &b : Bonds) // C++0x
		delete b;
};

bool Molecule::setBondsLinear (double eps, double H)
{
	unsigned long int ia = 0;
	for (auto &a : Atoms) // C++0x
	{
		DPRINT ("Atom '%s' (serial %lu, #%lu) is not bonded\n", a->getName(), a->getSerial(), ia);

		if ( a->getNBonds() == 0 )
		{ 
			// unbonded atom, bond to the neighbouring atoms from serials
			unsigned int s = a->getSerial ();
			DPRINT ("Atom '%s' (serial %lu, #%lu) is not bonded\n", a->getName(), s, ia);
			if (getNAtoms() == 1)
				AtomAttorney::setType (*a, ATOM_SINGLE);

			else if ( (ia == 0) && (getNAtoms() > 1) )
			{	
				// connect only if molecules in series
				DPRINT ("\tit is a head atom\n");
				Atom * next = Atoms.at(ia+1);
				if (next->getSerial() == s + 1)
				{
					Bonds.push_back (new Bond (a, next, eps, H));
					AtomAttorney::setType (*a, ATOM_HEAD);
				}
			}

			else if ( (ia == getNAtoms() - 1) && (getNAtoms() > 1) )
			{
				DPRINT ("\tit is a terminal atom\n");
				Atom * prev = Atoms.at(ia-1);
				if (prev->getSerial() == s-1)
				{
					Bonds.push_back (new Bond (a, prev, eps, H));
					AtomAttorney::setType (*a, ATOM_TERMINAL);
				}
			}

			else
			{
				DPRINT ("\tit is a bonded atom\n");
				Atom * next = Atoms.at(ia+1);
				if (next->getSerial() == s + 1)
				{
					Bonds.push_back (new Bond (a, next, eps, H));
					AtomAttorney::setType (*a, ATOM_BONDED);
				}
				Atom * prev = Atoms.at(ia-1);
				if (prev->getSerial() == s-1)
				{
					Bonds.push_back (new Bond (a, prev, eps, H));
					AtomAttorney::setType (*a, ATOM_BONDED);
				}
			}
		}
		ia++;
	}

	return true;
}



// Position
// set (virtual method, default to check and warn)
bool Molecule::setPosition (const Point3D& R)
{
	if (head->positionPtr()) 
	{
		BCPT_WARNING ("Position is set, use Molecule::moveTo() to reset.");
		return false;
	}

	BCPT_WARNING ("Virtual method, not implemented for a general molecule.");
	return false;
}

// translate molecule by a vector
bool Molecule::translateBy (const Point3D& T) 
{
	Point3D * po = head->positionPtr();
	if (!po) 
	{
		BCPT_WARNING ("Position not set, cannot translate.");
		return false;
	}

	// if the head position is set, we expect athe other atoms' positions to be set
	// so throw an exception if this is not so
	for (auto &a : Atoms) // C++0x
		if (!AtomAttorney::translateBy(*a, T))
			throw "Translating atom failed in translateBy()";

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
	for (auto &a : Atoms) // C++0x
		AtomAttorney::setSerial (*a, serial++);

	return serial;
}

// Overlap 
bool Molecule::overlap (const Atom & ta) const
{

	if (!ta.positionIsSet())
		throw "Molecule::overlap(): Atom's position is not set." ;

	if (!head->positionIsSet())
		throw "Molecule::overlap(): Molecule's position is not set." ;

	for (auto &a : Atoms) // C++0x
		if ( a->overlap(ta) )
			return true;

	return false;
}

bool Molecule::overlap (const Molecule & M) const
{
	if (!M.positionIsSet())
		throw "Molecule::overlap(): Test's molecule position is not set." ;

	if (!head->positionIsSet())
		throw "Molecule::overlap(): Moleculke's position is not set." ;

	for (auto &a : Atoms) // C++0x
		if ( M.overlap(*a) )
			return true;

	return false;
}

// For each loop
bool Molecule::foreachAtom (AtomFunction func, void * data) const
{
	for (auto &a : Atoms) // C++0x
		if (!func(*a, data))
			return false;
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
	*stream << "Molecule '" << name << "' has " << getNAtoms() << " atom(s) and " << getNBonds() << " bond(s). ";
//	*stream << "Total charge " << getCharge() << ", size (" << radius[0] << ", " << radius[1] << ", " << radius[2] << ")." << std::endl ;
	*stream << "Total charge " << getCharge() << std::endl ;

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
	for (auto &a : Atoms) // C++0x
		a->printBBStr (stream);

	for (auto &b : Bonds) // C++0x
		b->printBBStr (stream);

}

