
TESTS = demoncore
check_PROGRAMS = $(TESTS)
demoncore_SOURCES = core.cpp SystemTest.cpp
demoncore_CXXFLAGS = -I${top_srcdir}/src $(CPPUNIT_CFLAGS)
demoncore_LDFLAGS = $(CPPUNIT_LIBS) ${top_builddir}/src/libdemon.la
noinst_HEADERS = SystemTest.h
