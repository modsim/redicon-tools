/**************************************************************************
 *                                                                        *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defines.h"

#define UNERR(str)  {fprintf( stderr,"%s\n",str); return 0;}

int check_box (double ra[3], double rb[3], double H[3], int * box)
{
	double d[] = {0,0,0};

	int i;
	for (i =0; i < 3; i++)
	{
		/* null means infinity */
		if (H[i] == 0.)
			continue;

		d[i]  = 2. * (ra[i] - rb[i]);
		if ( H[i] < fabs (d[i]) )
		{
			box[i] = box[i] + d[i] / fabs(d[i]);
			DPRINT ("switching to box %i\n", box[i]);
		}
	}

	return 1;
}

int molb2mdat (FILE * f, int nr, int isdouble, const double R0[], double H[], FILE * of)
{
	unsigned int b[] = {0,0,0,0};
	unsigned int step_nr, mols, state_size;
	unsigned int size = 0;
	double tmp[13];
	double out[13];
	float *transf;
	// go to the beginning
	rewind (f);

	int box[3] = {0,0,0};
	double Rcur[3];
	double Rprev[3];
	double R00[3]; /* this is R at t=0 */

	int i;
	for (i = 0; i < 3; i++)
		R00[i] = R0[i];

	if (isdouble == 0)
		size = sizeof(float);
	else
		size = sizeof(double);

	for(;;)
	{
		if ( fread( b, sizeof(b), 1, f ) != 1 ) 
			break;
		if ( fread( &step_nr, sizeof(unsigned int), 1, f ) != 1 ) 
			UNERR("End of file step_nr");
		if ( fread( &mols, sizeof(unsigned int), 1, f ) != 1 ) 
			UNERR("End of molb file");
		DPRINT ("nmols=%d\n", mols);
		if ( mols <= nr )
		{
			fprintf(stderr,"Number of molecules (%d) is less of equal than the number given (%d)\n",mols,nr);
			return 0;
		}

		/* state - what's that? */
		if ( fread( &state_size, sizeof(unsigned int), 1, f ) != 1 ) 
			UNERR("End of file state_size");
		if ( fseek( f, state_size, SEEK_CUR ) != 0 ) 
			UNERR("End of file, fseek");

		/* time */
		double curr_time;
		/*if ( fseek( f, size, SEEK_CUR ) != 0 ) 
			UNERR("End of file, fseek");*/
		if ( fread( &curr_time, size, 1, f ) != 1 )
			UNERR("End of file curr_time");
		DPRINT (stderr, "time=%g\n", curr_time);

		/* translation matrix */
		if ( fseek( f, size*nr*4, SEEK_CUR ) != 0 ) 
			UNERR("End of file, fseek");
		if ( fread( tmp, size*4, 1, f ) != 1 ) 
			UNERR("End of file, read trans");
		if ( fseek( f, size*(mols-1-nr)*4, SEEK_CUR ) != 0 ) 
			UNERR("End of file, fseek");

		/* rotation matrix */
		if ( fseek( f, size*nr*9, SEEK_CUR ) != 0 ) 
			UNERR("End of file, fseek");
		if ( fread( tmp+4, size*9, 1, f ) != 1 ) 
			UNERR("End of file, read trans");
		if ( fseek( f, size*(mols-1-nr)*9, SEEK_CUR ) != 0 ) 
			UNERR("End of file, fseek");
		if ( isdouble )
		{
			int i;
			for ( i = 0; i < 13; ++i ) out[i] = tmp[i];
		}
		else
		{
			int i;
			transf = (float*) tmp;
			for ( i = 0; i < 13; ++i ) out[i] = transf[i];
		}
		
		int i;
		for (i = 0; i < 3; i++)
		{
			Rcur[i] = out[4+i] * R0[0] + out[7+i] * R0[1] + R0[2] * out[10+i];
			Rcur[i] += out[i];
		}

		if (curr_time == 0.0)
		{
			for (i = 0; i < 3; i++)
				R00[i] = Rcur[i];
		}


		check_box (Rprev, Rcur, H, box);
		double dr2 = 0.;
		for (i = 0; i < 3; i++)
		{
			Rprev[i] = Rcur[i]; /* save original Rcur */
			Rcur[i] += box[i] * H[i];
			double xi = Rcur[i] - R00[i];
			dr2 += pow2(xi);
		}

		fprintf(of, "% 1.10e   % 1.10e  % 1.10e  % 1.10e     % 1.10e  % 1.10e  % 1.10e\n", curr_time,
			Rcur[0], Rcur[1], Rcur[2],
			sqrt(dr2), dr2, curr_time == 0.0 ? 0.0 : dr2/curr_time); 

			/*	out[0],out[1],out[2],
				out[4],out[5],out[6],out[7],out[8],out[9],out[10],out[11],out[12]
				1       2      3       4     5      6       7      8        9) */
	}

	return 0;
}

