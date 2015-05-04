#!/usr/bin/env python2
#
#  analyze-xyz.py 2013 S Kondrat aka Valiska <valiska@gmail.com>
#                      Pradeep Burla <pradeepburla91@gmail.com>
#
#  Read the XYZ file and output geo centre position, displaysment, 
#  and mean-square displacemnt, and the 'diffusion coefficient' as a function of time
#
#  Usage: analyze-xyz.py -f FILE ...
#  Output: <time> <geometrical center position = 3 floats>  <dr>  <dr2 (=msd)> <Deff>
#
import os
import sys
import fileinput
import operator
import math
import string
import time
import datetime
import numpy as np

# Store value for cmd line parser
def store_value(option, opt_str, value, parser):
    setattr(parser.values, option.dest, value)

# Check if we leave the box of size H, and 
# update/shift the box along the coresponding
# axis; ra and rb are the two subsequent points
# before and after a move
def check_box (H, ra, rb, box):
	d = [0 for col in range(3)] 

	for i in range(3):

		# null means infinity
		if H[i] == 0.:
			continue;

		d[i]  = 2. * (ra[i] - rb[i])
		if H[i] < math.fabs(d[i]):
			if debug:
				print 'Switching from box %i along %i' % (box[i], i)
			box[i] = box[i] + d[i] / math.fabs(d[i])
			if debug:
				print 'To box %i' % (box[i])
	if debug:
		print 'done check_box()'

	return;


# Command line parser
from optparse import OptionParser
parser = OptionParser()

parser.add_option("-f", "--file", help="file name with data for eta", metavar="FILE", type="string", dest="filename")
parser.add_option("-k", "--line-from", help="track atoms from given line (0,1,...)", metavar="VAL", type="int", dest="k")
parser.add_option("-n", "--number-of-lines", help="track given number of lines", metavar="VAL", type="int", dest="n")
parser.add_option("-l", "--lines", help="track atoms from comma-separated lines", metavar="VAL", type="int", dest="l")
parser.add_option("-H","--box-size", action="callback", callback=store_value, metavar="Hx Hy Hz", type="int", nargs=3, dest="H")
parser.add_option("-a","--all",action="store_true",help="whether to make a track file for each particle",dest="do_all")

# Grab options
(options, args) = parser.parse_args()

if options.filename:
	filename = options.filename
else:
	print "The file name is missing (-f/--file)"
    	sys.exit()
if options.do_all:
	do_all=True
else:
	do_all=False
if options.k:
	k = options.k
else:
	k = 0;

if options.n:
	n = options.n
else:
	n = 0;

if options.l:
	print "Option not impelemnted, try -k and -n."
    	sys.exit()

# Box size 
if options.H:
	H = options.H
else:
	# zero mens infinity
	H = [0 for col in range(3)]

##########################
### Script starts here ###
##########################

debug = False
#debug = True

# Runnig vars
nline=iline = itime = size = natoms = 0
xo = yo = zo = 0.0;

#to calculate number of itimes for the dimesion of matrix
for line in fileinput.input(filename):
	nline=nline+1

ntime=(nline)/(natoms+2)

# Time and centre of a 'molecule' (natoms beads) position
time = [];
rx = []; 
ry = []; 
rz = []; 

# Position sof Tracking atoms
# set all to zero
Rx = [[0 for col in range(ntime)] for row in range(n)]
Ry = [[0 for col in range(ntime)] for row in range(n)]
Rz = [[0 for col in range(ntime)] for row in range(n)]

# Integer denoting the box whith the traced atoms
# Start from 0,0,0 and shift if leaving the box 
# in one of three directions
box = [[0 for col in range(3)] for row in range(n)]

# Check if we have PBC
if H[0] != 0 or H[1] != 0 or H[2] != 0:
	PBC = True
else:
	PBC = False

if do_all:
	arr_data=np.zeros((nline, 5)) #this is a bit larger than necessary
	arrcnt=0
	print >> sys.stderr, "set up numpy array w. shape: ", arr_data.shape
# Parse the input file
for line in fileinput.input(filename):
	if debug:
		print '***************************'
		print 'line %i: %s' % (iline, line)
		print 'itime = %i, size=%i,' % (itime, size)
		print '	=> %i, %i' %  (iline/(size+2), iline%(size+2))

	# if a line with the number of atoms
	if iline % (size + 2) == 0:
		tokens = line.split()
		if iline == 0:
			size = int(tokens[0])	
			if n == 0:
				n = size;
		else:
			# FIXME:  do same for all splits etc?
			size_read = 0
			try:
				size_read = int(tokens[0])
			except ValueError:
				print '# ***        Exception        ***' 
				print '# Error reading line %i' % (iline)
				print '# Cannot convert to integer, I give up reading the file'
				print '# The data is analyzed only up to the above line' 
				print '# *** End of exception message ***' 
				print '#'
				break

			if size_read != size:
				print 'Error reading line %i, wrong number of atoms, %i, expected %s.' % (iline, size, tokens[0])
			    	sys.exit()

	# if a comment line, has time in ps if produced by BD_BOX 
	elif iline % (size + 2) == 1 or iline == 1:
		
		if debug:
			print 'Averaging: iline =%i, itime = %i, size=%i => %i' % (iline, itime, size, iline/(size+2))

		if do_all==False and iline != 1:
			if natoms != n:
				print 'Internal error, number of atoms read %i and expected %i are not the same :(' % (natoms, n)
    				sys.exit()
			 	
			rx.append (xo/(natoms));
			ry.append (yo/(natoms));
			rz.append (zo/(natoms));
			itime = itime + 1
			natoms = 0;
			xo = yo = zo = 0.0;

		tokens = line.split();
		if len(tokens) > 3:
			# this is if created by BD_BOX
			time.append ( (float(tokens[3])) )
		else:
			time.append ( float(itime) )
	elif do_all:
		if iline%1000000==0:
			print >> sys.stderr, "parsing do_all line %d"%(iline)
		tokens=line.split()
		arr_data[arrcnt][3]=time[-1]
		arr_data[arrcnt][4]=float((iline % (size+2))-2)
		for ii in range(3):
			arr_data[arrcnt][ii]=float(tokens[ii+1])
		arrcnt+=1
	# if atom's position == lines of interest
	elif do_all==False and (iline - itime * (size + 2) - 2 >= k and iline - itime * (size + 2) - 2 < k + n):
		natoms = natoms + 1
		
		if debug:
			print 'line %i, atom %i' % (iline, natoms)
		tokens = line.split()
		
		Rx[natoms-1][itime] = float(tokens[1])
		Ry[natoms-1][itime] = float(tokens[2])
		Rz[natoms-1][itime] = float(tokens[3])
		
		if itime == 0:
			xo += float(tokens[1])
			yo += float(tokens[2])
			zo += float(tokens[3])

		elif itime > 0:
			r1 = [Rx[natoms-1][itime-1], Ry[natoms-1][itime-1], Rz[natoms-1][itime-1]]
			r2 = [Rx[natoms-1][itime], Ry[natoms-1][itime], Rz[natoms-1][itime]]
			b = box[natoms-1]
		
			if debug:
				print box

			check_box(H, r1, r2, b)
			box[natoms-1] = b;

			if debug:
				print box
				print

			xo += b[0] * H[0] + float(tokens[1])
			yo += b[1] * H[1] + float(tokens[2])
			zo += b[2] * H[2] + float(tokens[3])
	iline += 1
b=np.zeros(3)
if do_all==True:
	print >> sys.stderr, "now doing do_all loop"
	for ii in range(size):
		print >> sys.stderr, "processing file %d of %d"%(ii+1,size)
		fn_out=filename.replace(".xyz","")+".mol%d.dat"%(ii)
		fn_out=os.path.basename(fn_out)
		arr_ii=arr_data[arr_data[:,4]==float(ii)]
		for row_idx,row in enumerate(arr_ii[:]):
			if row_idx>0:
				r1=r2[:]
				r2=row[:3]
				check_box(H, r1,r2,b)
				r2+=b*H
			else:
				r2=row[:3]
				ref=r2[:]
		dr2=np.sum((arr_ii[:,:3]-ref)**2,axis=1)
		dr=np.sqrt(dr2)
		Deff=10. * dr2 / (6. * arr_ii[:,3])
		Deff[0]=0. # fixes the none from divide by zero	
		f_out=open(fn_out,"w")
		t = datetime.datetime.now()
		print >> f_out,	'# Created by analyze-xyz.py on %s at %s from %s' % (t.strftime("%d-%m-%Y"), t.strftime("%H:%M:%S %Z"), filename)
		print >> f_out, '# System sconsists of %i atoms' % (size)
		if PBC:
			print >> f_out, '# Periodic boundary applied with box size (%g, %g, %g)' % (H[0], H[1], H[2])
		print >> f_out, '# Tracking %i bead(s) starting from %ith' % (1, ii)
		print >> f_out, '# time (ps)    rx (A)   ry (A)   rz (A)    dr (A)    dr2 (A^2)    D (mum^2/s)'
		for row,dr_i,dr2_i,Deff_i in zip(arr_ii[:],dr,dr2,Deff):
			print >> f_out, ' %f    %e   %e   %e    %e   %e   %e' % (row[3], row[0], row[1], row[2], dr_i, dr2_i, Deff_i )
		f_out.close()
	sys.exit()
# Add the last step to positions
if natoms != n:
	print 'Internal error, number of atoms read %i and expected %i are not the same :(' % (natoms, n)
    	sys.exit()
else:
	rx.append (xo/(n));
	ry.append (yo/(n));
	rz.append (zo/(n));

# reference point (t = itime = 0)
x0 = rx[0]
y0 = ry[0]
z0 = rz[0]

# Save info lines to the file
t = datetime.datetime.now()
print '# Created by analyze-xyz.py on %s at %s from %s' % (t.strftime("%d-%m-%Y"), t.strftime("%H:%M:%S %Z"), filename)
print '# System sconsists of %i atoms' % (size)
if PBC:
	print '# Periodic boundary applied with box size (%g, %g, %g)' % (H[0], H[1], H[2])
print '# Tracking %i bead(s) starting from %ith' % (n, k)
print '# time (ps)    rx (A)   ry (A)   rz (A)    dr (A)    dr2 (A^2)    D (mum^2/s)'

# run over time and calculate dr, msd, etc
for j in range(0, len(time), 1):

	dr2 = math.pow (x0 - rx[j], 2) + math.pow (y0 - ry[j], 2) + math.pow (z0 - rz[j], 2)
	dr = math.sqrt (dr2)

	if j != 0:
		Deff = 10. * dr2 / (6. * time[j])
	else:
		Deff = 0.0;
	print ' %f    %e   %e   %e    %e   %e   %e' % (time[j], rx[j], ry[j], rz[j], dr, dr2, Deff )

