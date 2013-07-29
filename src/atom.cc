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

Atom::Atom (char * name, int serial, double radius, double charge) 
	: name(name), serial(serial), radius(radius), charge(charge), 
	type (ATOM_FREE), nneighbours(0), neighbours(NULL), nbonds(0), bonds(NULL),
	userData(NULL)
	{};

Atom::~Atom () {};


char * Atom::getType () const
{
	char * t = NULL;
	switch (type)
	{
		case ATOM_FREE: strdup("free"); break;

		case ATOM_BONDED: strdup("bonded"); break;
		
		case ATOM_HEAD: strdup("head"); break;

		default: strdup("unknown") ;

	}
	return t;
}

//
// print Atom's info
//
void Atom::print (char * name) const
{
	std::ofstream file;
	file.open(name);

	print (&file);

	file.close();
}


void Atom::print (std::ofstream * file) const
{
	*file << name << ": " << getType() << " # " << serial << " has " << nneighbours << " and " << nbonds << "nbonds" << std::endl ;
}

