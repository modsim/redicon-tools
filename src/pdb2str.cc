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
using namespace std;

static const char * myname = "pdb2str";
void usage () 
{
	fprintf (stderr, "Usage: %s -p PDB -s STR\n", myname);
	fprintf (stderr, "where options are\n");

	fprintf (stderr, " -p, --pdb-file=FILE          input PDB file name.\n");
	fprintf (stderr, " -s, --str-file=FILE          output file name for BD_BOX's str file format.\n");

	fprintf (stderr, " -v, --version                print version iformation and exit.\n");
	fprintf (stderr, " -h, --help                   print this message and exit.\n");

	return;
};

int main (int argc, char ** argv) 
{
	char * pdb = (char*) NULL;
	FILE * pdb_file = (FILE*) NULL;

	char * str = (char*) NULL;
	FILE * str_file = (FILE*) NULL;

	/* 
	 * Command-line parser 
	 */  

	int c = 0;

#define ARGS    "hvp:s:"
	while (c != EOF)  {
#ifdef HAVE_GETOPT_LONG
		int option_index = 0;
		static struct option long_options[] =  {

			{"help", no_argument, NULL, 'h'},
			{"version", no_argument, NULL, 'v'},

			{"pdb-file", required_argument, NULL, 'p'},
			{"str-file", no_argument, NULL, 's'},

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
	if (!pdb_file || !str_file)
	{
		cerr << myname << ": error: no input and/or output file, use "  << myname << " --help for help." << endl;
		return 1; /* failure */

	}
	

	//
	// Free everythiong
	//
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

