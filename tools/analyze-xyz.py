#!/usr/bin/env python2
#
#  analyze-xyz.py S Kondrat aka Valiska <valiska@gmail.com> 2013
#
#  Read the XYZ file and output geo centre position, displaysment, 
#  and mean-square displacemnt, and the 'diffusion coefficient' as a function of time
#
#  Usage: analyze-xyz.py -f FILE 
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

# Command line parser
from optparse import OptionParser
#set up command-line options
parser = OptionParser()

parser.add_option("-f", "--file", help="file name with data for eta", metavar="FILE", type="string", dest="filename")
parser.add_option("-k", "--line-from", help="track atoms from given line", metavar="VAL", type="int", dest="k")
parser.add_option("-n", "--to-line", help="track atoms to given line", metavar="VAL", type="int", dest="n")
parser.add_option("-l", "--lines", help="track atoms from comma-separated lines", metavar="VAL", type="int", dest="l")

# Grab options
(options, args) = parser.parse_args()

if options.filename:
	filename = options.filename
else:
	print "The file name is missing (-f/--file)"
    	sys.exit()

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


debug = False

# Time and centre of a 'molecule' position
time = [];
rx = []; 
ry = []; 
rz = []; 

# Runnig vars
iline = itime = size = 0
xo = yo = zo = 0.0;

# Print when and from what file
t = datetime.datetime.now()
print '# Created by analyze-xyz.py on %s at %s from %s' % (t.strftime("%d-%m-%Y"), t.strftime("%H:%M:%S %Z"), filename)

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
			print '# System sconsists of %i atoms' % (size)
			if n == 0:
				n = size;
		else:
			if int(tokens[0]) != size:
				print 'Error reading line %i, wrong number of atoms, %i, expected %s.' % (iline, size, tokens[0])
			    	sys.exit()

	# if a comment line, has time in ps if produced by BD_BOX 
	elif iline % (size + 2) == 1 or iline == 1:
		
		if debug:
			print 'Averaging: iline =%i, itime = %i, size=%i => %i' % (iline, itime, size, iline/(size+2))

		if iline != 1:
			rx.append (xo/(n-k)); 	
			ry.append (yo/(n-k)); 
			rz.append (zo/(n-k)); 

			itime = itime + 1
			xo = yo = zo = 0.0;

		tokens = line.split();
		if len(tokens) > 3:
			# this is if created by BD_BOX
			time.append ( (float(tokens[3])) )
		else:
			time.append ( float(itime) )

	# if atom - position lines of interest
	elif iline - itime * (size - 2) > k and iline - itime * (size - 2) < n:
		if debug:
			print '%i' % (iline)
		tokens = line.split()
		xo += float(tokens[1])
		yo += float(tokens[2])
		zo += float(tokens[3])

	iline += 1

# Add the last step to positions
rx.append (xo/(n-k)); 	
ry.append (yo/(n-k)); 
rz.append (zo/(n-k)); 

# reference point (t = itime = 0)
x0 = rx[0]
y0 = ry[0]
z0 = rz[0]

print '# time (ps)    rx (A)   ry (A)   rz (A)    dr (A)    dr2 (A^2)    D (mum^2/s)'
for j in range(0, len(time), 1):

	dr2 = math.pow (x0 - rx[j], 2) + math.pow (y0 - ry[j], 2) + math.pow (z0 - rz[j], 2)
	dr = math.sqrt (dr2)

	if j != 0:
		Deff = 10. * dr2 / (6. * time[j])
	else:
		Deff = 0.0;
	print ' %f    %e   %e   %e    %e   %e   %e' % (time[j], rx[j], ry[j], rz[j], dr, dr2, Deff )

