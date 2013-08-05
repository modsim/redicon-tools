/*  moleculePDBread.cc 2013-08-02 valiska@gmail.com
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

Molecule::Molecule (const char * file) : nBonds(0), bonds(NULL), owner (NULL)
{

	Molecule::name = strdup (file);
	std::ifstream stream (file);
	
	if (stream.rdstate() != std::ios_base::goodbit) 
		throw "Cannot open file." ;

	std::string line;

	while (!stream.eof())
	{
		std::getline (stream, line);
		if (line.find ("ATOM", 0) != std::string::npos)
		{
			Atoms.push_back (new Atom(line)); // ignore throw as we check above
		}
	}

	head = Atoms.at (0);

	stream.close ();

	charge = 0;
	for (auto &a : Atoms) // C++0x
	{
		charge += a->getCharge();
		AtomAttorney::claimOwnership (*a, *this);
		AtomAttorney::setType (*a, ATOM_SINGLE);
	}

#ifdef DEBUG
	for (auto &a : Atoms) // C++0x
		a->printInfo (&std::cerr);
#endif

};

