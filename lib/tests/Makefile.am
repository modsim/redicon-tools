check_PROGRAMS = test-coord test-atom test-molecule test-system

AM_CPPFLAGS= -Wall -DBCPT_MAINTAINER=\"@BCPT_MAINTAINER@\" -I$(top_srcdir)/lib/
AM_LDFLAGS =  -Wall -pedantic -ansi 
#-fwritable-strings 

test_coord_SOURCES = testCoord.cc 
test_coord_LDADD = $(top_srcdir)/lib/libMol.la

test_atom_SOURCES = testAtom.cc 
test_atom_LDADD = $(top_srcdir)/lib/libMol.la

test_molecule_SOURCES = testMolecule.cc 
test_molecule_LDADD = $(top_srcdir)/lib/libMol.la

test_system_SOURCES = testSystem.cc 
test_system_LDADD = $(top_srcdir)/lib/libMol.la
