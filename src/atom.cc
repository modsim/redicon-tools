/*  atom.cc 2013-07-29 valiska@gmail.com
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

#include <stddef.h>
#include <string.h>
#include <iostream> 

#include<atom.h>

#include "defines.h"

Atom::Atom (const char * name, int serial, double radius, double charge) 
	: serial(serial), radius(radius), charge(charge), r(NULL), 
	type (ATOM_FREE), nneighbours(0), neighbours(NULL), nbonds(0), bonds(NULL),
	userData(NULL)
{ 
	Atom::name = strdup (name);
};

Atom::~Atom () 
{
	delete name;
	if (r) delete r;
	if (neighbours) delete neighbours;
	if (bonds) delete bonds;
};

char * Atom::getType () const
{
	char * t = NULL;
	switch (type)
	{
		case ATOM_FREE: t = strdup("free"); break;

		case ATOM_BONDED: t = strdup("bonded"); break;
		
		case ATOM_HEAD: t = strdup("head"); break;

		default: t = strdup("unknown") ;
	}
	return t;
}

//
// print Atom's info
//
void Atom::printInfo (char * name) const
{
	std::ostream * stream = new std::ofstream (name);
	printInfo (stream);
	delete stream;
}

void Atom::printInfo (std::ostream * stream) const
{
	*stream << name << " (serial " << serial << ") is "  << getType() << " and has " << nneighbours << " neighbours and " << nbonds << " nbonds" << std::endl ;
}

void Atom::printBBStr (std::ostream * stream) const
{
	*stream << "sub " << name << " " << serial;
	if (r)
		r->print(stream);
	else
	{
		BCPT_WARNING ("atom %s (%i): no coordinates, setting to zeros!", name, serial);
		*stream << " 0.0 0.0 0.0 " ;
	}
	*stream << charge << " " << radius << std::endl ;
}

