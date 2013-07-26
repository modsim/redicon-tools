SUBDIRS = tests

bin_PROGRAMS = pdb2str

#noinst_LIBRARIES = libMol.a 
#libMol_a_SOURCES = read_line.c

AM_CPPFLAGS= -Wall -DBCPT_MAINTAINER=\"@BCPT_MAINTAINER@\" -I$(top_srcdir)/src/
AM_LDFLAGS = -pedantic -ansi -fwritable-strings

pdb2str_SOURCES = pdb2str.cc
#pdb2str_LDADD = libMol.a 

EXTRA_DIST = defines.h 
#BUGS TODO FIXMES
