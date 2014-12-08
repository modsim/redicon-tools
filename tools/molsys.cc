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
#include <monatomic.h>

#include "defines.h"

using namespace std;
//#define DEBUG

static const char * myname = "molsys-create";
void usage () 
{
	fprintf (stderr, "Usage: %s \n", myname);
	fprintf (stderr, "create a system of uni-atom molecules and output to an .str file.\n");
	fprintf (stderr, "Options are\n");

	fprintf (stderr, " -H, --size=X,Y,Z        system size (in A).\n");
	fprintf (stderr, " -R, --location=X,Y,Z    system's location, ie box center (in A).\n");

	fprintf (stderr, " -n, --names=NAME,NAME...   names of atoms.\n");
	fprintf (stderr, " -r, --radia=VAL,VAL...     radia (in A).\n");
	fprintf (stderr, " -N, --numbers=VAL,VAL,...  number of particles.\n");

	fprintf (stderr, " -q, --charge=VAL,VAL,...   charges (in e).\n");
	fprintf (stderr, " -m, --masses=VAL,VAL,...   masses (arbitrary units).\n");
	fprintf (stderr, " -d, --hd-radia=VAL,VAL,... hydrodynamic radia (in A).\n");
	fprintf (stderr, " -U, --use-hd-radia         use HD radia when packing in a box.\n");

	fprintf (stderr, " -j, --lj=VAL,VAL,...       Lennard-Jones parameters (in kcal/mol).\n");
	fprintf (stderr, " -T, --type=VAL,VAL,...     placement type (rand, cent, lat).\n");
	fprintf (stderr, "                  cent can be used once for a single particle.\n");
	fprintf (stderr, " -t, --tries=N            number of tries (for -a rand) before giving up.\n");

	fprintf (stderr, " -s, --str-file=FILE      STR output file.\n");
	fprintf (stderr, " -v, --version            print version iformation and exit.\n");
	fprintf (stderr, " -h, --help               print this message and exit.\n");

	return;
};

void setRandomPosition (const Point3D & H, const Point3D & R, Point3D * p)
{
	double r[3];
	for (int i = 0; i < 3; i++)
		r[i] = R.get(i) - 0.5 * H.get (i) + H.get (i) * rand() / (RAND_MAX + 1.0);
	p->set (r);
#ifdef DEBUG
	p->print (stderr, "p");
#endif
}

Point3D getRandomPosition (const Point3D & H, const Point3D & R, double a)
{
	double r[3];
	for (int i = 0; i < 3; i++)
		r[i] = R.get(i) - 0.5 * H.get (i) + a + (H.get (i) - 2. * a) * rand() / (RAND_MAX + 1.0);
	Point3D p(r);

#ifdef DEBUG
	p.print (stderr, "p");
#endif

	return p;
}


int main (int argc, char ** argv) 
{
	Point3D * H = NULL;
	Point3D * R = NULL;

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

	double * charges = NULL;
	unsigned int ncharges = 0;

	double * HDradia = NULL;
	unsigned int nHDradia = 0;
	bool useHDRadia = false;

	double * masses = NULL;
	unsigned int nmasses = 0;

	double * LJ = NULL;
	unsigned int nLJ = 0;

	char ** types = NULL;
	unsigned int ntypes = 0 ;

	//char * arrangment = NULL;

	string str_file;
	
	char * pdb = (char*) NULL;
	FILE * pdb_file = (FILE*) NULL;

	int ntries = 100;

	/* 
	 * Command-line parser 
	 */ 

	int c = 0;

#define ARGS    "hvH:R:n:r:N:q:d:Uj:m:s:T:t:"
	while (c != EOF)  {
#ifdef HAVE_GETOPT_LONG
		int option_index = 0;
		static struct option long_options[] =  {

			{"help", no_argument, NULL, 'h'},
			{"version", no_argument, NULL, 'v'},

			//{"pdb-file", required_argument, NULL, 'p'},
			{"str-file", required_argument, NULL, 's'},

			{"size", required_argument, NULL, 'H'},
			{"location", required_argument, NULL, 'R'},

			{"names", required_argument, NULL, 'n'},
			{"radia", required_argument, NULL, 'r'},
			{"numbers", required_argument, NULL, 'N'},

			{"charges", required_argument, NULL, 'q'},
			{"hd-radia", required_argument, NULL, 'd'},
			{"use-hd-radia", no_argument, NULL, 'U'},
			{"lj", required_argument, NULL, 'j'},
			{"masses", required_argument, NULL, 'm'},
			{"type",required_argument, NULL,'T'},

			{"tries", required_argument, NULL, 't'},
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

				case 's': str_file = strdup (optarg);
					  break;

				case 'U': useHDRadia = true;
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
						  H = new Point3D (Hd[0], Hd[1], Hd[2]);
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
						  R = new Point3D (Rd[0], Rd[1], Rd[2]);
						  free (Rd);
					  }
					break;

				case 'n':  names = str2strlist (&optarg, ",", &nnames); 
					   for (unsigned int i = 0; i < nnames; i++)
						   for (unsigned int j = i+1; j < nnames; j++)
							   if ( strlen(names[i]) == strlen (names[j]) )
								   if ( strncmp (names[i], names[j], strlen (names[i])) == 0)
								   {
									   fprintf (stderr, "%s: error: atom names '%s' and '%s' are the same.\n", myname, names[i], names[j]);
									   return 1;
								   }
				break;

				case 'r':  nradii = str2dlist (&optarg, ",", &radii); break;

				case 'N':  nN = str2dlist (&optarg, ",", &N); break;

				// optional options
				case 'q':  charges = (double *) malloc (sizeof (double) );
					ncharges = str2dlist (&optarg, ",", &charges); break;

				case 'd':  HDradia = (double *) malloc (sizeof (double) );
					nHDradia = str2dlist (&optarg, ",", &HDradia); break;

				case 'm':  masses = (double *) malloc (sizeof (double) );
					nmasses = str2dlist (&optarg, ",", &masses); break;

				case 'j':  LJ = (double *) malloc (sizeof (double) );
					nLJ = str2dlist (&optarg, ",", &LJ); break;

				case 't':  ntries = atoi (optarg); break;

				// usage/version
				case 'h': usage (); exit (1); break;

				case 'v': printf ("%s %s\n", myname, VERSION); exit (1); break;

				case '?': /* wrong options */
					  fprintf (stderr, "Try `%s --help' for more information.\n", myname);
					  return 1; /* failure */
			
                case 'T':  types = str2strlist (&optarg, ",", &ntypes);break;


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
	
	if ( (nradii != nN) || (nN != nnames) )
	{
		cerr << myname << ": error: number of molecule's names (" << nnames<< "), radii("<< nradii << "), and their numbers (" << nN << ") in the system must be the same."  << endl;
		return 1; /* failure */
	}

	if ( (charges) && (ncharges != nnames))
	{
		cerr << myname << ": error: number of molecule's charges must equal the number of molecule types (see -n, -r and -N)."  << endl;
		return 1; /* failure */
	}

	if ( (masses) && (nmasses != nnames))
	{
		cerr << myname << ": error: number of molecule's masses must equal the number of molecule types (see -n, -r and -N)."  << endl;
		return 1; /* failure */
	}
	if ( (HDradia) && (nHDradia != nnames))
	{
		cerr << myname << ": error: number of molecule's HD radia must equal the number of molecule types (see -n, -r and -N)."  << endl;
		return 1; /* failure */
	}
	if ( (LJ) && (nLJ != nnames))
	{
		cerr << myname << ": error: number of molecule's LJ parameters must equal the number of molecule types (see -n, -r and -N)."  << endl;
		return 1; /* failure */
	}

	if ( (types) && (ntypes != nnames) )
	{
		cerr << myname << ": error: number of placement types must equal the number of molecule types (see -n, -r and -N)."  << endl;
		return 1; /* failure */
	}

// Create system
	System S (*R, *H);
	unsigned int nMols = 0;
	for (unsigned int i = 0; i < nN; i++)
		nMols += N[i];


	int have_centr = 0;
	if (types) 
	{
		for (unsigned int i = 0;i<ntypes;i++)
			if (strstr (types[i], "cent"))
			{	
				if( N[i] != 1 ) 
				{
					cerr << myname << ": error: placement "<< types[i] << ": more than one (" << N[i] << ") molecule of type " << names[i] << " must be one."  << endl;
					return 1;	
				}	
				if (have_centr)
				{
					cerr << myname << ": error: there is already a molecule in the center"  << endl;
					return 1;	
				}	

				Atom a (names[i], radii[i]);

				if (useHDRadia)
					a.overlap_use_HDRadius();

				if (charges) a.setCharge (charges[i]);
				if (LJ) a.setLJ (LJ[i]);
				if (HDradia) a.setHDRadius (HDradia[i]);
				if (masses) a.setMass (masses[i]);

				Monatomic * M = new Monatomic (names[i], a);
				M->setPosition (*R);
				if ( !S.addMolecule (*M) )
				{
					cerr << myname << ": error: cannot add a molecule in the center, giving up."  << endl;
					return 1;	
				}		
				have_centr++;

			}
			else if (!strstr (types[i],"rand"))
			{
				cerr << myname << ": error: unknown or unimplemented placement type."  << endl;
				return 1;
			}
	}

	for (unsigned int i = 0; i < nN; i++)
	{	
		Atom a (names[i], radii[i]);
		if (useHDRadia)
			a.overlap_use_HDRadius();
		if (charges) a.setCharge (charges[i]);
		if (LJ) a.setLJ (LJ[i]);
		if (HDradia) a.setHDRadius (HDradia[i]);
		if (masses) a.setMass (masses[i]);

		for (int j = 0; j < N[i]; j++)
		{
			// "cent" molecule is already in the box, skip
			if (types && strstr (types[i], "cent"))
				break;

			Monatomic * M = new Monatomic (names[i], a);
			int counter = 0;
			while (1)
			{			
				Point3D p = getRandomPosition (*H, *R, radii[i]);
				M->setPosition (p);
				counter++;
				if ( S.addMolecule (*M) )
					break;
				if (counter > ntries)
				{
					BCPT_ERROR ("cannot add molecule '%s' after %i tries, I am giving up.", M->getName(), counter);
					delete M;
					break;
				}
			}
		}
	}	

	std::cerr << "Packing info: " << nMols << " molecules given, " << S.getNMolecules() << " packed in a box." << std::endl;

	if ( !str_file.empty()) 
	{
		string info_file = str_file + ".info";
		S.printInfo ((char*) info_file.c_str());
		S.printBBStr (str_file.c_str());
	}
	else
	{
		S.printInfo(&std::cerr);
		S.printBBStr(&std::cout);
	}

	//
	// Free everything
	//

	delete H;
	delete R;

	free (radii);
	free (N);
	for (unsigned int i = 0; i < nnames; i++)
		free (names[i]);
	free (names);

	if (charges ) free (charges);
	if (masses) free (masses);
	if (HDradia) free (HDradia);
	if (LJ) free (LJ);

	for (unsigned int i = 0; i < ntypes; i++)
		free (types[i]);
	if (types) free (types);


	if (pdb_file)
		free (pdb_file);

	if (pdb)
		free (pdb);

	S.tryDelete();

	if (nMols != S.getNMolecules())
		return 1;
	else
		return 0;
}

