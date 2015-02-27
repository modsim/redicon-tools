/**************************************************************************
 *   Copyright 2015 Svyatoslav Kondrat aka Valiska
 *   This code is based on parsemolb originaly copyrighted by 
 *   Copyright (C) 2012,2013: University of Warsaw                        *
 *   Authors:            Pawel Zielinski <pzielins@cent.uw.edu.pl>        *
 *                       Maciej Dlugosz  <mdlugosz@cent.uw.edu.pl>        *
 *                                                                        *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 3 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *   This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program; if not, see http://www.gnu.org/licenses     *
 *   or write to the Free Software Foundation,Inc., 51 Franklin Street,   *
 *   Fifth Floor, Boston, MA 02110-1301  USA                              *
 *                                                                        *
 **************************************************************************/

#include "config.h"
#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#endif /* HAVE_GETOPT_H */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "str.h"

int molb2mdat (FILE * f, int nr, int isdouble, const double R0[], const double H[], FILE * of);

static const char * myname = "molb2dat";
void usage () 
{
	fprintf (stderr, "Usage: %s \n", myname);
	fprintf (stderr, "Extract positions and time from .molb data of BD_BOX.\n");
	fprintf (stderr, "Only the whole molecules (ie geometrical centers) are analyzed at the moment.\n");
	fprintf (stderr, "The resulting data file per molecule is: time x y z dr dr2 dr2/time.\n");

	fprintf (stderr, "Options are\n");
	fprintf (stderr, " -i, --input-file=FILE        molb file name\n");
	fprintf (stderr, " -o, --output-file=FILE       output base file name (output: FILE.mol$n.dat)\n");
	fprintf (stderr, " -m, --molecules=N0[,Nn,Ns]   extract for molecules N0 to Nn (skiping Ns)\n");
	fprintf (stderr, " -r, --position=X,Y,Z         initial position of the molecule in question");
	fprintf (stderr, "                              (geo center from .mstr file; in A).\n");
	fprintf (stderr, " -H, --size=X,Y,Z             size of a simulation box (in A).\n");

	//fprintf (stderr, " -s, --str-file=FILE      STR file .mstr (instead of -r option).\n");
	fprintf (stderr, " -v, --version            print version iformation and exit\n");
	fprintf (stderr, " -h, --help               print this message and exit\n");

	return;
};


int main( int argc, char* argv[] )
{
	FILE * molbfile;
	char * obase = (char*) NULL;
	char * molbname = (char*) NULL;

	unsigned int Nm[3] = {0,0,0};
	int isdouble =  1;
/*
	if ( strcmp( argv[3], "float" ) == 0 ) 
		isdouble = 0; 
	else if ( strcmp( argv[3], "double" ) == 0 ) 
		isdouble = 1; 
	else 
	{
		fprintf (stderr, "Wrong variable type (float, double)\n");
		return 0;
	}
*/
	double R0[] = {0., 0., 0.};
	double H[] = {250., 250., 250.};

	/* 
	 * Command-line parser 
	 */ 

	int c = 0;

#define ARGS    "hvH:r:m:i:o:"
	while (c != EOF)  {
#ifdef HAVE_GETOPT_LONG
		int option_index = 0;
		static struct option long_options[] =  {

			{"help", no_argument, NULL, 'h'},
			{"version", no_argument, NULL, 'v'},

			//{"mstr-file", required_argument, NULL, 's'},

			{"size", required_argument, NULL, 'H'},
			{"position", required_argument, NULL, 'r'},
			{"molecules", required_argument, NULL, 'm'},

			{"input-file", required_argument, NULL, 'i'},
			{"output-file",required_argument, NULL,'o'},
		};

		switch ((c = getopt_long (argc, argv, ARGS, long_options, &option_index))) 
#else 
		switch ((c = getopt (argc, argv, ARGS))) 
#endif /* HAVE_GETOPT_LONG */
			{
				case 'i': 
					  molbname = strdup (optarg);
					  molbfile = fopen (molbname, "r");
					  if (!molbfile)
					  {
						  fprintf (stderr, "%s: error: caanot open file '%s' for reading.\n", 
						  	myname, molbname);
						  return 1;
					  }
					  break;

				case 'o': obase = strdup (optarg);
					  break;

				case 'H': {
						  double * Hd = (double*) malloc ( 1 * sizeof (double) );
						  int nHd = str2dlist (&optarg, ",", &Hd);
						  int i;
						  for (i = 0; i < nHd; i++)
							  H[i] = Hd[i];
						  if (nHd == 1)
						  {
							  H[1] = H[2] = Hd[0];
						  }
						  else if (nHd != 3)
						  {
							  free(Hd);
							  fprintf (stderr, "%s: error: box position incorrect, use '%s -H x,y,z'.\n", 
							  myname, myname);
						  }

						  free (Hd);
					  }
					break;

				case 'r': {
						  double * Rd = (double*) malloc ( 1 * sizeof (double) );
						  int nRd = str2dlist (&optarg, ",", &Rd);
						  int i;
 						  for (i = 0; i < nRd; i++)
							  R0[i] = Rd[i];
						  if (nRd == 1)
						  {
						  	R0[1] = R0[2] = Rd[0];
						  }
						  else if (nRd != 3)
						  {
							  free(Rd);
							  fprintf (stderr, "%s: error: molecules position incorrect, use '%s -r x,y,z'.\n", myname, myname);
							  return 1;

						  }
						  free (Rd);
					  }
					break;

				case 'm':  
					{
						  int * Nd = (int*) malloc ( 1 * sizeof (int) );
						  int nNd = str2intlist (&optarg, ",", &Nd);
						  int i;
 						  for (i = 0; i < nNd; i++)
							  Nm[i] = Nd[i];
						  if (nNd == 1)
						  {
						  	Nm[1] = Nd[0] + 1;
							Nm[2] = 1;
						  }
						  else if (nNd == 2)
						  {
							Nm[2] = 1;
						  }
						  else if (nNd != 3)
						  {
							  free(Nd);
							  fprintf (stderr, "%s: error: molecules position incorrect, use '%s -m m0[.m1,ms]'.\n", myname, myname);
							  return 1;

						  }
						  free (Nd);
					  }
				break;

				// usage/version
				case 'h': usage (); exit (1); break;

				case 'v': printf ("%s %s\n", myname, VERSION); exit (1); break;

				case '?': /* wrong options */
					  fprintf (stderr, "Try `%s --help' for more information.\n", myname);
					  return 1; /* failure */
			}  
	}  	

	if (!molbname)
	{
	  fprintf (stderr, "%s: error: no molb file, use '%s -i FILE'.\n", myname, myname);
	  return 1;
	}

	if (!obase)
	{
	  fprintf (stderr, "%s: error: no output file name, use '%s -o FILE'.\n", myname, myname);
	  return 1;
	}

	int i = 0;
	for (i = Nm[0]; i < Nm[1]; i += Nm[2])
	{
		char ofname_size = (strlen (obase) + 12) * sizeof(char);
		char * ofname = (char*) malloc ( ofname_size );
		snprintf (ofname, ofname_size, "%s.mol%d.dat", obase, i);
		FILE * of = fopen( ofname, "w" );
		if (!of)
		{
			fprintf (stderr, "Error: cannot open file '%s' for writing, writing to stdout\n", ofname);
			of = stdout;
		}
		
		printf ("Saving molecule %i to file %s\n", i, ofname);
		time_t rawtime;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		fprintf (of, "# Created by %s %s from %s on %s", myname, VERSION, molbname, asctime (timeinfo) );
		fprintf (of, "# time (ps)          x (A)              y (A)              z(A)                  dr (A)             dr2 (A^2)          dr2/time (A^2/ps)\n");

		molb2mdat (molbfile, i, isdouble, R0, H, of);
		fclose (of);
	}
	fclose (molbfile);

	return 0;
}

