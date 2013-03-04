TESTS = ising stoch

check_PROGRAMS = ising stoch

noinst_HEADERS = clocker/ClockerListener.h \
			clocker/ClockerXmlHook.h \
			clocker/Timer.h
			ReservoirBenchmark.h

# Shared variables

SOURCES_PERFORMANCE = clocker/ClockerListener.cpp \
                        clocker/ClockerModel.cpp \
			clocker/ClockerXmlHook.cpp \
			clocker/Timer.cpp \
			ReservoirBenchmark.cpp

CXXFLAGS_PERFORMANCE = $(CPPUNIT_CFLAGS) \
			-I ${top_srcdir}/src

LDADD_PERFORMANCE = ${top_builddir}/src/libdemon.la

LIBS_PERFORMANCE = $(CPPUNIT_LIBS) \
			$(GSL_LIBS)

# Ising Performance Test:

ising_SOURCES = ising.cpp \
			$(SOURCES_PERFORMANCE)

ising_CXXFLAGS = $(CXXFLAGS_PERFORMANCE)

ising_LDADD = $(LDADD_PERFORMANCE)

ising_LDFLAGS =  $(CPPUNIT_LIBS) \
		$(GSL_LIBS)

# Stochastic Performance Test:

stoch_SOURCES = stoch.cpp \
			$(SOURCES_PERFORMANCE)

stoch_CXXFLAGS = $(CXXFLAGS_PERFORMANCE)

stoch_LDADD = $(LDADD_PERFORMANCE)

stoch_LDFLAGS =  $(CPPUNIT_LIBS) \
                $(GSL_LIBS)
