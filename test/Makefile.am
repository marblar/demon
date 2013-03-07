if PERFORMANCE_TESTS_ENABLED
PERFORMANCE_DIR = performance
endif

SUBDIRS = . $(PERFORMANCE_DIR)

TESTS = demoncore

check_PROGRAMS = $(TESTS)

noinst_HEADERS = TestFixtures.h MockObjects.h

demoncore_SOURCES = SystemTest.cpp \
			MeasurementTest.cpp \
			IsingTest.cpp \
			StochTest.cpp

demoncore_CXXFLAGS = -I${top_srcdir}/src/lib \
			$(CPPUNIT_CFLAGS) \
			$(GCOV_CFLAGS) \
			$(GSL_CFLAGS)

demoncore_LDFLAGS = $(BOOST_LDFLAGS)

demoncore_LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIB) \
			${top_builddir}/src/libdemon.la \
			$(GSL_LIBS)

clean-local:
	rm -f *.xml
