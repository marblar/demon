check_PROGRAMS = demoncore

noinst_HEADERS = TestFixtures.h MockObjects.h

demoncore_SOURCES = SystemTest.cpp \
			MeasurementTest.cpp \
			IsingTest.cpp \
			StochTest.cpp \
			core.cpp \
			IsingGridTest.cpp \
			RandomnessDelegateTest.cpp \
			CAToolsTest.cpp \
			TMGasTest.cpp

demoncore_CXXFLAGS = -I${top_srcdir}/src/lib \
			$(CPPUNIT_CFLAGS) \
			$(GCOV_CFLAGS) \
			$(GSL_CFLAGS)

demoncore_CPPFLAGS = -DBOOST_TEST_DYN_LINK

demoncore_LDFLAGS = $(BOOST_LDFLAGS)

demoncore_LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIB) \
			${top_builddir}/src/libdemon.la \
			$(GSL_LIBS)

check-targets: demoncore
