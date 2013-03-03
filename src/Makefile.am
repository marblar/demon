noinst_LTLIBRARIES = libdemon.la
noinst_HEADERS = Stochastic.h \
			Ising.h \
			Reservoir.h \
			System.h \
			Utilities.h \
			InstrumentFactories.h \
			clangomp.h \
			Measurement.h

bin_PROGRAMS = ${top_builddir}/jdemon

__top_builddir__jdemon_CXXFLAGS = $(GCOV_CFLAGS)\
		  $(OPENMP_CXXFLAGS)\
		  $(BOOST_CXXFLAGS)\
		  $(GSL_CXXFLAGS) 

__top_builddir__jdemon_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS) \
		 $(GSL_LDFLAGS)

__top_builddir__jdemon_LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
	       $(GSL_LIBS) \
	       $(GCOV_LIBS) \
	       libdemon.la

__top_builddir__jdemon_SOURCES = main.cpp

libdemon_la_SOURCES = Stochastic.cpp \
			Ising.cpp \
			Reservoir.cpp \
			System.cpp \
			Utilities.cpp \
			InstrumentFactories.cpp \
			Measurement.cpp

libdemon_la_CXXFLAGS = $(GCOV_CFLAGS)

libdemon_la_LIBADD = $(GSL_LIBS) \
		     $(GCOV_LIBS)
