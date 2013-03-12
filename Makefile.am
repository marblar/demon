AUTOMAKE_OPTIONS = foreign
SUBDIRS = src test scripts
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -g -O3 -Wall 
AM_CPPFLAGS = -I${top_srcdir}/src

EXTRA_DIST = $(top_srcdir)/.version                                                                 
BUILT_SOURCES = $(top_srcdir)/.version                                                              

$(top_srcdir)/.version:                                                                             
	echo $(VERSION) > $@-t && mv $@-t $@

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version  

if DEMON_GCOV_ENABLED
include test/coverage/Makemodule.am
endif

