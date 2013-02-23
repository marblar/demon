AM_CXXFLAGS=-g -O2 -fopenmp $(BOOST_CXXFLAGS)$(GSL_CFLAGS) $(OPENMP_CXXFLAGS) $(BOOST_SYSTEM_FLAGS)
AM_LDFLAGS = -fopenmp $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(GSL_LDFLAGS) $(BOOST_TIMER_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS)
LIBS += $(BOOST_PROGRAM_OPTIONS_LIBS) $(GSL_LIBS) $(BOOST_TIMER_LIBS) $(BOOST_SYSTEM_LIBS)

bin_PROGRAMS=jdemon
jdemon_SOURCES=main.cpp Stochastic.cpp Ising.cpp Reservoir.cpp System.cpp Utilities.cpp ReservoirFactory.cpp
noinst_HEADERS = clangomp.h Stochastic.h Ising.h Reservoir.h System.h Utilities.h ReservoirFactory.h

ACLOCAL_AMFLAGS = -I build-aux/m4
