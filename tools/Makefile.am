bin_PROGRAMS = pdb2str molsys-create

bin_SCRIPTS= analyze-xyz.py average-me.py fit-pow.sh
EXTRA_DIST = $(bin_SCRIPTS)

AM_CPPFLAGS= -Wall -DBCPT_MAINTAINER=\"@BCPT_MAINTAINER@\" -I$(top_srcdir)/lib/
AM_LDFLAGS = -pedantic -ansi 

pdb2str_SOURCES = pdb2str.cc str.c
pdb2str_LDADD = $(top_srcdir)/lib/libMol.la

molsys_create_SOURCES = molsys.cc str.h str.c 
molsys_create_LDADD = $(top_srcdir)/lib/libMol.la
