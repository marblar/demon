TESTS = ising stoch

check_PROGRAMS = ising stoch

noinst_HEADERS = clocker/ClockerListener.h \
			clocker/ClockerXmlHook.h \
			clocker/Timer.h \
			ReservoirBenchmark.h

LDADD = ${top_builddir}/src/libdemon.la

AM_CPPFLAGS = -I${top_srcdir}/src/lib \
		$(GSL_CFLAGS) \
		$(CPPUNIT_CFLAGS)

AM_LDFLAGS = $(CPPUNIT_LIBS) \
		$(GSL_LIBS)

# Shared variables

SOURCES_PERFORMANCE = clocker/ClockerListener.cpp \
                        clocker/ClockerModel.cpp \
			clocker/ClockerXmlHook.cpp \
			clocker/Timer.cpp \
			ReservoirBenchmark.cpp

# Ising Performance Test:

ising_SOURCES = ising.cpp \
			$(SOURCES_PERFORMANCE)

# Stochastic Performance Test:

stoch_SOURCES = stoch.cpp \
			$(SOURCES_PERFORMANCE)
