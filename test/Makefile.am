if PERFORMANCE_TESTS_ENABLED
PERFORMANCE_DIR = performance
endif

SUBDIRS = . $(PERFORMANCE_DIR)

TESTS = demoncore

check_PROGRAMS = $(TESTS)

noinst_HEADERS = SystemTest.h MeasurementTest.h StochTest.h IsingTest.h

demoncore_SOURCES = core.cpp \
			SystemTest.cpp \
			MeasurementTest.cpp \
			IsingTest.cpp \
			StochTest.cpp

demoncore_CXXFLAGS = -I${top_srcdir}/src/lib \
			$(CPPUNIT_CFLAGS) \
			$(GCOV_CFLAGS) \
			$(GSL_CFLAGS)

demoncore_LDADD = $(CPPUNIT_LIBS) \
			${top_builddir}/src/libdemon.la \
			$(CPPUNIT_LIBS) \
			$(GSL_LIBS)

clean-local:
	rm -f *.xml
