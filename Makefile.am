if DEMON_GCOV_ENABLED
COVERAGE_DIR=coverage
endif

AUTOMAKE_OPTIONS = foreign
SUBDIRS = src test $(COVERAGE_DIR)
ACLOCAL_AMFLAGS = -I build-aux/m4
AM_CXXFLAGS = -g -O3 -Wall 
AM_CPPFLAGS = -I${top_srcdir}/src
