/*  moleculePQR.cc 2013-08-06 valiska@gmail.com copied from moleculePDB.cc
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

bool Molecule::readPQR (std::ifstream * stream)
{
	std::string line;
#ifdef DEBUG
	unsigned int ia = 0;
#endif

	while (!stream->eof())
	{
		std::getline (*stream, line);
		if ( (line.find ("ATOM", 0) != std::string::npos) || (line.find ("HETATM", 0) != std::string::npos) )
		{	
			Atom * a;
			try {
				a = new Atom(FILETYPE_PQR, line);
			} catch (const char * msg) {
				return false;
			}
			DPRINT ("Adding %i th atom\n", ++ia);
			Atoms.push_back (a);
		}
	}
	DPRINT ("DONE: no more atoms\n");
#ifdef DEBUG
	ia = 0;
	for (auto &a : Atoms)
	{
		DPRINT ("atom %u\n", ++ia);
		a->printInfo (&std::cerr);
	}
#endif

	return true;


};

