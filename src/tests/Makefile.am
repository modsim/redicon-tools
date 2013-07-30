check_PROGRAMS = test-coord test-atom test-molecule

AM_CPPFLAGS= -Wall -DBCPT_MAINTAINER=\"@BCPT_MAINTAINER@\" -I$(top_srcdir)/src/
AM_LDFLAGS =  -Wall -pedantic -ansi -fwritable-strings 

test_coord_SOURCES = testCoord.cc 
test_coord_LDADD = $(top_srcdir)/src/libMol.la

test_atom_SOURCES = testAtom.cc 
test_atom_LDADD = $(top_srcdir)/src/libMol.la

test_molecule_SOURCES = testMolecule.cc 
test_molecule_LDADD = $(top_srcdir)/src/libMol.la
