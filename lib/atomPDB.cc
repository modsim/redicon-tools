/*  atomString.cc 2013-08-02 valiska@gmail.com
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

//#include <iostream>
//#include <fstream>
#include <string>
#include <stdexcept>      // std::out_of_range
#include <string.h> // strdup();

#include<atom.h>

// For PDB ATOM see http://deposit.rcsb.org/adit/docs/pdb_atom_format.html#ATOM

//#define DEBUG
#include "defines.h"

bool Atom::readPDB (const std::string & line) 
{ 

#ifdef DEBUG
	std::cerr << "Atom::readPDB(): '" << line << "'" << std::endl;
#endif
	std::string str;

	str = line.substr (0,6);
	if ( (str.find ("ATOM", 0) == std::string::npos) && (str.find ("HETATM", 0) == std::string::npos) )
	{
		BCPT_ERROR ("Not an ATOM or HETATM record");
		return false;
	}

	str = line.substr (6, 5);
	serial = atoi(str.c_str());

	str = line.substr (12, 4);
	name = strdup(str.c_str());

 	// FIXME: we ignore chars from 17 to 31

	str = line.substr (30, 8);
	double x = atof (str.c_str());

	str = line.substr (38, 8);
	double y = atof (str.c_str());

	str = line.substr (46, 8);
	double z = atof (str.c_str());
	r = new Point3D (x, y, z);

	try {
		str = line.substr (79,2);
		charge = atof (str.c_str());

	} catch (const std::out_of_range & oor) {
		BCPT_WARNING ("No charge: string out of range, setting to zero.");
		charge = 0;
	}
	return true;
};

