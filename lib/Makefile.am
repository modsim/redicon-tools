SUBDIRS = tests

noinst_LTLIBRARIES = libMol.la
libMol_la_SOURCES = coord3D.cc coord3D.h \
	atom.h atom.cc str.c atomString.cc \
	molecule.h molecule.cc \
	system.h system.cc

AM_CPPFLAGS= -Wall -DBCPT_MAINTAINER=\"@BCPT_MAINTAINER@\" -I$(top_srcdir)/lib/
AM_LDFLAGS = -pedantic -ansi -fwritable-strings

EXTRA_DIST = defines.h 
#BUGS TODO FIXMES
