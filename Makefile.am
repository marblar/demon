AUTOMAKE_OPTIONS = foreign
#SUBDIRS = src scripts
ACLOCAL_AMFLAGS = -I build-aux/m4
AM_CXXFLAGS = -g -O3 -Wall 
AM_CPPFLAGS = -I${top_srcdir}/src

bin_PROGRAMS =
noinst_LIBRARIES =
noinst_HEADERS =
noinst_LTLIBRARIES =

include src/Makemodule.am
include test/Makemodule.am
#include scripts/Makefile.am
