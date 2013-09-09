/*  pdb2str.cc  2013-07-26     transform PDB to STR format
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

#include "config.h"
#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#endif /* HAVE_GETOPT_H */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <molecule.h>
#include "str.h"

static const char * myname = "pdb2str";
void usage () 
{
	fprintf (stderr, "Usage: %s -p PDB -s STR\n", myname);
	fprintf (stderr, "where options are\n");

	fprintf (stderr, " -p, --pdb-file=FILE          input PDB or PQR file name.\n");
	fprintf (stderr, " -s, --str-file=FILE          output file name for BD_BOX's str file format.\n");
	fprintf (stderr, " -b, --bonds=EPS,H            bind atoms if not bonded with deviation eps\n"
	                 "                              and the 'spring constant' H.\n");
	fprintf (stderr, " -a, --angle=H                make bond angles along a 'chain' (serial numbers)\n"
	                 "                              with the 'spring constant' H.\n");
	fprintf (stderr, " -i, --info                   print info about the molecule.\n");

	fprintf (stderr, " -v, --version                print version iformation and exit.\n");
	fprintf (stderr, " -h, --help                   print this message and exit.\n");

	return;
};

int main (int argc, char ** argv) 
{
	std::string pdb;
	std::string str;

	double * BondInfo = NULL;
	int nBondInfo = 0;
	double AngleH = 0.0;

	bool printInfo = false;
	/* 
	 * Command-line parser 
	 */  

	int c = 0;

#define ARGS    "hvip:s:b:a:"
	while (c != EOF)  {
#ifdef HAVE_GETOPT_LONG
		int option_index = 0;
		static struct option long_options[] =  {

			{"help", no_argument, NULL, 'h'},
			{"version", no_argument, NULL, 'v'},

			{"pdb-file", required_argument, NULL, 'p'},
			{"str-file", required_argument, NULL, 's'},
			{"info", no_argument, NULL, 'i'},
			{"bonds", required_argument, NULL, 'b'},
			{"angles", required_argument, NULL, 'a'},
		};

		switch ((c = getopt_long (argc, argv, ARGS, long_options, &option_index))) 
#else 
			switch ((c = getopt (argc, argv, ARGS))) 
#endif /* HAVE_GETOPT_LONG */
			{
				case 'p': 
					  pdb = std::string (optarg);
					  break;
				case 's': 
					  str = std::string (optarg);
					  break;
					  break;
				case 'b':
					  BondInfo = (double*) malloc ( 2 * sizeof (double));
					  nBondInfo = str2dlist (&optarg, ",", &BondInfo);
						  if (nBondInfo != 2)
						  {
							  free(BondInfo);
							  fprintf (stderr, "%s: error: -b option requeres two argments: bond separation relative to the read separations, and the spring constant, use '%s -H EPS,H'\n", myname, myname);
							  return 1;

						  }
					  break;

				case 'a': AngleH= atof (optarg);
					  break;

				case 'i': printInfo = true;
					  break;

				case 'h': usage (); exit (1); break;

				case 'v': printf ("%s %s\n", myname, VERSION); exit (1); break;

				case '?': /* wrong options */
					  fprintf (stderr, "Try `%s --help' for more information.\n", myname);
					  return 1; /* failure */
			}  
	}  	

	//
	// Analyze options
	//
	if (pdb.empty())
	{
		std::cerr << myname << ": error: no input file, use "  << myname << " --help for help." << std::endl;
		return 1; /* failure */

	}
	
	Molecule * M;
	if (pdb.rfind(".pdb") != std::string::npos)
		try {
			M = new Molecule (FILETYPE_PDB, pdb.c_str());
		} catch (const char * msg) {
			printf ("Error cautgh: %s\n", msg);
			return 1;
		}
	else if (pdb.rfind(".pqr") != std::string::npos)
		try {
			M = new Molecule (FILETYPE_PQR, pdb.c_str());
		} catch (const char * msg) {
			printf ("Error cautgh: %s\n", msg);
			return 1;
		}

	else
	{
		std::cerr << myname << ": error: unknown file extension " << pdb << std::endl;
		return 1;
	}

	if (BondInfo)
		M->setBondsLinear (BondInfo[0], BondInfo[1]);

	if (AngleH)
		M->setAnglesLinear (ANGLE_POTTYPE_SQUARE, AngleH);

	if (!str.empty())
	{
		std::cerr << "writing to a STR file " << str << std::endl;
		M->printBBStr (str.c_str());
	}

	if (printInfo)
		M->printInfo (&std::cout);

	// Free everythiong
	delete M;

	if (BondInfo)
		free (BondInfo);

	return 1;
}

