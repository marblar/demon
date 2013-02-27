AM_CXXFLAGS=-g -O3

bin_PROGRAMS=jdemon
jdemon_CXXFLAGS = -fopenmp $(BOOST_CXXFLAGS)$(GSL_CFLAGS) $(OPENMP_CXXFLAGS) $(BOOST_SYSTEM_FLAGS)
jdemon_LDFLAGS = -fopenmp $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(GSL_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS)
jdemon_LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) $(GSL_LIBS) $(BOOST_TIMER_LIBS) $(BOOST_SYSTEM_LIBS)

jdemon_SOURCES=main.cpp Stochastic.cpp Ising.cpp Reservoir.cpp System.cpp Utilities.cpp ReservoirFactory.cpp Circos.cpp
noinst_HEADERS = clangomp.h Stochastic.h Ising.h Reservoir.h System.h Utilities.h ReservoirFactory.h Circos.h

ACLOCAL_AMFLAGS = -I build-aux/m4
