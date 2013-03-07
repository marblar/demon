TESTS = ising stoch

check_PROGRAMS = ising stoch

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I${top_srcdir}/src/lib \
		$(GSL_CFLAGS) \
		-DBOOST_TEST_DYN_LINK \
		-DBOOST_TEST_MODULE

AM_LDFLAGS = $(BOOST_LDFLAGS) \
		$(GSL_LIBS)

LDADD = $(GSL_LIBS) \
		$(BOOST_UNIT_TEST_FRAMEWORK_LIB) \
		${top_builddir}/src/libdemon.la

# Ising Performance Test:

clean-local:
	rm -f *.stats
