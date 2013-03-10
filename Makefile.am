if DEMON_GCOV_ENABLED
COVERAGE_DIR=coverage
endif

AUTOMAKE_OPTIONS = foreign
SUBDIRS = src test $(COVERAGE_DIR)
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -g -O3 -Wall 
AM_CPPFLAGS = -I${top_srcdir}/src

EXTRA_DIST = $(top_srcdir)/.version                                                                 
BUILT_SOURCES = $(top_srcdir)/.version                                                              

$(top_srcdir)/.version:                                                                             
	echo $(VERSION) > $@-t && mv $@-t $@

clean-local:
	-rm .version

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version  

coverage : all check
	make -C coverage coverage

