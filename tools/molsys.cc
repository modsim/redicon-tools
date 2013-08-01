/*  molsys.cc  2013-07-31   Create and save a system of molecules
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

#include "str.h"
#include <system.h>

using namespace std;
//#define DEBUG

static const char * myname = "molsys-create";
void usage () 
{
	fprintf (stderr, "Usage: %s \n", myname);
	fprintf (stderr, "create a system of molecules and output an .str file.\n");
	fprintf (stderr, "Options are\n");

	fprintf (stderr, " -H, --size=X,Y,Z        system size.\n");
	fprintf (stderr, " -R, --location=X,Y,Z    system's location.\n");

	fprintf (stderr, " -n, --name=NAME,NAME...   atom's names.\n");
	fprintf (stderr, " -r, --radius=VAL,VAL...   atom's radius.\n");
	fprintf (stderr, " -N, --number=VAL,VAL,...  number of particles.\n");
	fprintf (stderr, " -q, --charge=VAL,VAL,...  charges.\n");

	fprintf (stderr, " -a, --arrangment=rand/latt  arrangment (random/latt).\n");
	fprintf (stderr, " -t, --tries=N           number of tries (for -a rand) before giving up.\n");

	fprintf (stderr, " -v, --version            print version iformation and exit.\n");
	fprintf (stderr, " -h, --help               print this message and exit.\n");

	return;
};

void setRandomPosition (const Coord3D & H, const Coord3D & R, Coord3D * p)
{
	double r[3];
	for (int i = 0; i < 3; i++)
		r[i] = R.getCoord(i) - 0.5 * H.getCoord (i) + H.getCoord (i) * rand() / (RAND_MAX + 1.0);
	p->setCoord (r);
#ifdef DEBUG
	p->print (stderr, "p");
#endif
}

Coord3D getRandomPosition (const Coord3D & H, const Coord3D & R, double a)
{
	double r[3];
	for (int i = 0; i < 3; i++)
		r[i] = R.getCoord(i) - 0.5 * H.getCoord (i) + a + (H.getCoord (i) - 2. * a) * rand() / (RAND_MAX + 1.0);
	Coord3D p(r);

#ifdef DEBUG
	p.print (stderr, "p");
#endif

	return p;
}

int main (int argc, char ** argv) 
{
	Coord3D * H = NULL;
	Coord3D * R = NULL;

	char ** names = NULL;
	unsigned int nnames = 0;

	/* we alloc for one for str2dlist */
	double * radii = (double*) malloc ( sizeof(double) );
	unsigned int nradii = 0;

	/* C++ template for str2list<unsigned int>
	 * unsigned int * N = (unsigned int*) malloc ( sizeof(unsigned int) );
	unsigned int nN = 0;*/
	double * N = (double*) malloc ( sizeof(double) );
	unsigned int nN = 0;

	double * charges = (double*) malloc ( sizeof(double) );
	unsigned int ncharges = 0;

	//char * arrangment = NULL;

	char * str = (char*) NULL;
	FILE * str_file = (FILE*) NULL;
	
	char * pdb = (char*) NULL;
	FILE * pdb_file = (FILE*) NULL;

	/* 
	 * Command-line parser 
	 */  

	int c = 0;

#define ARGS    "hvH:R:n:r:N:q:a:t:s:"
	while (c != EOF)  {
#ifdef HAVE_GETOPT_LONG
		int option_index = 0;
		static struct option long_options[] =  {

			{"help", no_argument, NULL, 'h'},
			{"version", no_argument, NULL, 'v'},

			{"pdb-file", required_argument, NULL, 'p'},
			{"str-file", no_argument, NULL, 's'},

			{"size", no_argument, NULL, 'H'},
			{"location", no_argument, NULL, 'R'},

			{"name", no_argument, NULL, 'n'},
			{"radii", no_argument, NULL, 'r'},
			{"number", no_argument, NULL, 'N'},
			{"charge", no_argument, NULL, 'q'},

			{"arrangment", no_argument, NULL, 'a'},
			{"tries", no_argument, NULL, 't'},
		};

		switch ((c = getopt_long (argc, argv, ARGS, long_options, &option_index))) 
#else 
			switch ((c = getopt (argc, argv, ARGS))) 
#endif /* HAVE_GETOPT_LONG */
			{
				case 'p': 
					  pdb = strdup (optarg);
					  pdb_file = fopen (pdb, "r");
					  if (!pdb_file)
					  {
						  fprintf (stderr, "%s: error: caanot open file '%s' for reading.\n", myname, pdb);
						  return 1;
					  }
					  break;

				case 's': 
					  str = strdup (optarg);
					  str_file = fopen (pdb, "r");
					  if (!str_file)
					  {
						  fprintf (stderr, "%s: error: caanot open file '%s' for writing.\n", myname, str);
						  return 1;
					  }
					  break;

				case 'H': {
						  double * Hd = (double*) malloc ( 3 * sizeof (double) );
						  int nHd = str2dlist (&optarg, ",", &Hd);
						  if (nHd != 3)
						  {
							  free(Hd);
							  fprintf (stderr, "%s: error: box size incorrect, use '%s -H x,y,z'.\n", myname, myname);
							  return 1;

						  }
						  H = new Coord3D (Hd[0], Hd[1], Hd[2]);
						  free (Hd);
					  }
					break;

				case 'R': {
						  double * Rd = (double*) malloc ( 3 * sizeof (double) );
						  int nRd = str2dlist (&optarg, ",", &Rd);
						  if (nRd != 3)
						  {
							  free(Rd);
							  fprintf (stderr, "%s: error: box position incorrect, use '%s -H x,y,z'.\n", myname, myname);
							  return 1;

						  }
						  R = new Coord3D (Rd[0], Rd[1], Rd[2]);
						  free (Rd);
					  }
					break;

				case 'n':  names = str2strlist (&optarg, ",", &nnames); break;

				case 'r':  nradii = str2dlist (&optarg, ",", &radii); break;

				case 'q':  ncharges = str2dlist (&optarg, ",", &charges); break;

				case 'N':  nN = str2dlist (&optarg, ",", &N); break;

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
	if (!H || !R)
	{
		cerr << myname << ": error: no box's size or position, use "  << myname << " --help for help." << endl;
		return 1; /* failure */
	}
	
	if ( (nradii != ncharges) && (ncharges != nN) && (nN != nnames))
	{
		cerr << myname << ": error: number of molecule's names, radii, charges, and their number in the system is not the same."  << endl;
		return 1; /* failure */
	}


	System S (*R, *H);
	unsigned int nMols = 0;
	for (unsigned int i = 0; i < nN; i++)
		nMols += N[i];

	Molecule ** M = (Molecule**) malloc (nMols * sizeof (Molecule*) );

	int imol = 0;
	int ntries = 10;

	for (unsigned int i = 0; i < nN; i++)
	{
		Atom a (names[i], radii[i]);
		a.setCharge (charges[i]);
		for (int j = 0; j < N[i]; j++)
		{
			M[imol] = new Molecule (names[i], a);
			int counter = 0;
			while (1)
			{
				Coord3D p = getRandomPosition (*H, *R, radii[i]);
				M[imol]->setPosition (p);
				counter++;
				if ( S.addMolecule (*M[imol]) || (counter > ntries) )
					break;
			}
			imol++;
		}
	}

	cerr << nMols << " vs " << imol << endl;
	S.printInfo(&std::cerr);

	S.printBBStr(&std::cout);

	//
	// Free everything
	//

	delete H;
	delete R;

	free (radii);
	free (N);
	free (charges);
	for (unsigned int i = 0; i < nnames; i++)
		free (names[i]);
	free (names);

	for (unsigned int i = 0; i < nMols; i++)
		delete M[i];
	free (M);

	if (pdb_file)
		free (pdb_file);

	if (str_file)
		fclose (str_file);

	if (pdb)
		free (pdb);
	if (str)
		free (str);

	return 1;
}

