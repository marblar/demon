if PERFORMANCE_TESTS_ENABLED

PERFTEST = performance

endif

TESTS = demoncore $(PERFTEST)

check_PROGRAMS = $(TESTS)

demoncore_SOURCES = core.cpp \
			SystemTest.cpp \
			MeasurementTest.cpp

demoncore_CXXFLAGS = -I${top_srcdir}/src \
			$(CPPUNIT_CFLAGS) \
			$(GCOV_CFLAGS)

demoncore_LDFLAGS = $(CPPUNIT_LIBS) \
			${top_builddir}/src/libdemon.la

demoncore_LIBS = $(GCOV_LIBS) \
			$(CPPUNIT_LIBS)

EXTRA_PROGRAMS = performance

noinst_HEADERS = SystemTest.h \
			MeasurementTest.h 
			IsingPerformance.h
			clocker/ClockerListener.h
			clocker/ClockerXmlHook.h
			clocker/Timer.h

performance_SOURCES = performance.cpp \
			IsingPerformance.cpp \
			clocker/ClockerListener.cpp \
			clocker/ClockerModel.cpp \
			clocker/ClockerXmlHook.cpp \
			clocker/Timer.cpp 

performance_CXXFLAGS = $(CPPUNIT_CFLAGS) \
			-I ${top_srcdir}/src 

performance_LDADD = ${top_builddir}/src/libdemon.la

performance_LDFLAGS = ${CPPUNIT_LIBS} \
			${GSL_LIBS}

