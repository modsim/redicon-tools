/*  atom.cc 2013-07-29 valiska@gmail.com
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

//#include<atom.h>
#include<bond.h>

//#define DEBUG
#include "defines.h"

// simplified constructor 
Atom::Atom (const char * name, double radius) 
	: residue(NULL), molecule (NULL), residue_name (NULL),
	serial(0), hs_radius(radius), charge(0.0), 
	LJ (0.5), hd_radius (radius), mass (1.0), r(NULL), 
	type (ATOM_FREE), 
	userData(NULL)
{ 
	Atom::name = strdup (name);
	typeName = type2name ();
};

// full construct 
Atom::Atom (const char * name, unsigned int serial, const Point3D & r,
			double hs_radius, double hd_radius,
			double charge, double LJ, double mass)
	: residue(NULL), molecule (NULL),residue_name (NULL), 
	serial(serial), hs_radius(hs_radius), charge(charge), 
	LJ (LJ), hd_radius (hd_radius), mass (mass), 
	type (ATOM_FREE), 
	userData(NULL)
{
	Atom::name = strdup (name);
	typeName = type2name ();
	Atom::r = new Point3D (r);
};


// construct from the line
Atom::Atom (unsigned int ft, const std::string & line) 
	: residue(NULL), molecule (NULL), residue_name (NULL), 
	hs_radius(1.), LJ (0.5), hd_radius (1.), mass (1.0), 
	type (ATOM_FREE), 
	userData(NULL)
{ 

#ifdef DEBUG
	std::cerr << line << std::endl;
#endif

	switch (ft) {
	
		case FILETYPE_PQR: 
			if (!readPQR (line))
				throw "Atom::Atom(): Reading a PQR line failed";
			break;

		case FILETYPE_PDB: 
			if (!readPDB (line))
				throw "Atom::Atom(): Reading a PDB line failed";
			break;

		default:
			throw "Atom::Atom(): Unsupported or unimplemented file type";

	}
	typeName = type2name ();
}

// copy constructor (make a free brother)
Atom::Atom (const Atom & a) 
	: residue(NULL), molecule (NULL), residue_name (NULL),
	serial(0), hs_radius(a.getHSRadius()), charge(a.getCharge()), 
	LJ (a.getLJ()), hd_radius (a.getHDRadius()), mass (a.getMass()), 
	r(NULL), type (ATOM_FREE), 
	userData(NULL)
{ 
	Atom::name = strdup (a.getName());
	typeName = type2name ();
	Neighbours.clear();
	Bonds.clear();
};

// annihilator :P
Atom::~Atom () 
{
	free (name);
	if (residue_name)
		free (residue_name);
	if (typeName) free (typeName);
	if (r) delete r;
};

// Add a bond
bool Atom::addBond (Bond & b)
{
	Atom * n = b.getBondedAtom(this);
	if (n)
	{
		Neighbours.push_back (n);
		Bonds.push_back (&b);
		return true;
	}
	
	BCPT_ERROR ("Adding bond failed, atom '%s' does not belong to the bond", getName());
	return false;
}

bool Atom::addAngle (Angle & a)
{
	// FIXME: do checks etc
	Angles.push_back (&a);
	return true;
}

// Type to name etc
char * Atom::type2name () const
{
	char * t = NULL;
	switch (type)
	{
		case ATOM_FREE: t = strdup("free atom"); break;

		case ATOM_SINGLE: t = strdup("single atom"); break;

		case ATOM_BONDED: t = strdup("bonded atom"); break;

		case ATOM_TERMINAL: t = strdup("terminal atom"); break;

		case ATOM_HEAD: t = strdup("head atom"); break;

		default: t = strdup("unknown") ;
	}
	return t;
}

bool Atom::setType (AtomType T) 
{
	type = T; 
	free (typeName);
	typeName = type2name();
	return true;
}

// Position stuff
bool Atom::setPosition (const Point3D& R) 
{ 
	if (r)
	{
		BCPT_WARNING ("Position of atom '%s' has been set, resetting", name)
		delete r;
	}
	
	r = new Point3D (R); 
	if (!r) 
		return false; 
	else 
		return true;
};

bool Atom::unsetPosition() 
{ 
	if (r)
	{
		delete r;
	}
	
	r= NULL;
	return true;
};


bool Atom::translateBy (const Point3D& R) 
{ 
	if (!r)
	{
		BCPT_ERROR ("Position of atom '%s' has not been set, cannot translate", name)
		return false;
	}
	
	r->translateBy (R); 
	return true;
};

// Return copy of Point3D
Point3D * Atom::positionCopy () const
{
	if (r)
	{
		Point3D * rc = new Point3D (*r);
		return rc;
	}
	else
		return NULL;
}

// check for overlap
bool Atom::overlap (const Atom & a) const
{
	if (!r)
		throw "Atom's position not set";

	double dist_min =hs_radius + a.getHSRadius();
	Point3D * ra = a.positionCopy();

	if (!ra)
	{
		delete ra;
		throw "Test atom's position not set";
	}

	double dist = r->distanceTo (*ra);
	delete ra;
	
	DPRINT ("dist_min=%1.15e, dist=%1.15e\n", dist_min, dist);
	// FIXME: add some small eps?? -- cannot touch me, oh-oo-ho
	if (dist - 2.e-2 * dist_min <= dist_min )
		return true;
	else
		return false;
}

// Ownerships stuff
bool Atom::claimOwnership (Residue & R)
{
	if (residue)
	{
		BCPT_ERROR ("Ownership claim refused");
		return false;
	}
	residue = &R;
	return true;
}
bool Atom::claimOwnership (Molecule & M)
{
	if (molecule)
	{
		BCPT_ERROR ("Ownership claim refused");
		return false;
	}
	molecule = &M;
	return true;
}

// print basic Atom's info
void Atom::printInfo (char * name) const
{
	std::ostream * stream = new std::ofstream (name);
	printInfo (stream);
	delete stream;
}

void Atom::printInfo (std::ostream * stream) const
{
	*stream << "Atom '" << name << "' (serial " << serial << ") is a "  << typeName << " and has " << getNNeighbours() << " neighbour(s), " << getNBonds() << " bond(s)," << " and " << getNAngles() << " angle bond(s)."  << std::endl ;
}

void Atom::printBBStr (std::ostream * stream) const
{
	*stream << "sub " << name << " " << serial;
	if (r)
		r->print(stream);
	else
	{
		BCPT_WARNING ("atom %s (%li): no coordinates, setting to zeros!", name, serial);
		*stream << " 0.0 0.0 0.0 " ;
	}
	*stream << hd_radius << " " << charge <<  " " << 2. * hs_radius << " " << LJ << " " << mass << std::endl ;
}

