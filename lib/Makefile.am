SUBDIRS = tests

noinst_LTLIBRARIES = libMol.la
libMol_la_SOURCES = point3D.cc point3D.h \
	atom.h atom.cc atomPDB.cc \
	bond.h bond.cc \
	molecule.h molecule.cc moleculePDB.cc \
	monatomic.h \
	system.h system.cc

AM_CPPFLAGS= -std=c++0x -Wall -DBCPT_MAINTAINER=\"@BCPT_MAINTAINER@\" -I$(top_srcdir)/lib/
AM_LDFLAGS = -pedantic -ansi -fwritable-strings

EXTRA_DIST = defines.h 
#BUGS TODO FIXMES
