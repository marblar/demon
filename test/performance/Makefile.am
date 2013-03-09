check_PROGRAMS = ising stoch

TESTS = ising-wrapper.sh stoch-wrapper.sh
EXTRA_DIST = $(TESTS) junit.xslt

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I${top_srcdir}/src/lib \
		$(GSL_CFLAGS) \
		-DBOOST_TEST_DYN_LINK

AM_LDFLAGS = $(BOOST_LDFLAGS) \
		$(GSL_LIBS)

LDADD = $(GSL_LIBS) \
		$(BOOST_UNIT_TEST_FRAMEWORK_LIB) \
		${top_builddir}/src/libdemon.la

all-local: junit.xslt
	cp ${srcdir}/junit.xslt ${builddir}/junit.xslt

# Ising Performance Test:

clean-local:
	rm -f *.stats
