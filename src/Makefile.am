SUBDIRS = tests

bin_PROGRAMS = pdb2str

noinst_LTLIBRARIES = libMol.la
libMol_la_SOURCES = coord3D.cc coord3D.h  atom.cc atom.h
#
AM_CPPFLAGS= -Wall -DBCPT_MAINTAINER=\"@BCPT_MAINTAINER@\" -I$(top_srcdir)/src/
AM_LDFLAGS = -pedantic -ansi -fwritable-strings

pdb2str_SOURCES = pdb2str.cc
pdb2str_LDADD = libMol.la

EXTRA_DIST = defines.h 
#BUGS TODO FIXMES
