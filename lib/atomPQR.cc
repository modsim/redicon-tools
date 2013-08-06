/*  atomPQR.cc 2013-08-02 valiska@gmail.com
 *  get an Atom from PQR line
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

#define DEBUG
#include "defines.h"

bool Atom::readPQR (const std::string & line) 
{ 

#ifdef DEBUG
	std::cerr << "Atom::readPQR(): '" << line << "'" << std::endl;
#endif
	std::string str = line.substr (0,5);
	if ( (str.find ("ATOM", 0) == std::string::npos) && (str.find ("HEtATM", 0) == std::string::npos) )
	{
		BCPT_ERROR ("Not an ATOM or HETATM record");
		return false;
	}

	// assume a PQR line a space separated line with the needed values
	const char * cline = line.c_str();
	char keyword[6];
	name = (char*) malloc (4 * sizeof (char));
	char residue_name[3];
	unsigned int residue_number;
	double x,y,z;

	if ( sscanf (cline, "%s %u %s %s %u %lf %lf %lf %lf %lf", 
		&keyword, &serial, name, &residue_name, &residue_number, 
		&x, &y, &z, &charge, &hs_radius) != 10)
	{
		BCPT_ERROR ("Error parsing a PQR line");
		free (name);
		return false;
	}
	hd_radius = hs_radius;
	r = new Point3D (x, y, z);
	return true;
};

