TESTS = demoncore
check_PROGRAMS = $(TESTS)
demoncore_SOURCES = core.cpp\
			SystemTest.cpp

demoncore_CXXFLAGS = -I${top_srcdir}/src \
			$(CPPUNIT_CFLAGS) \
			$(GCOV_CFLAGS)

demoncore_LDFLAGS = $(CPPUNIT_LIBS) \
			${top_builddir}/src/libdemon.la

demoncore_LIBS = $(GCOV_LIBS) \
			$(CPPUNIT_LIBS)

noinst_HEADERS = SystemTest.h
