noinst_LTLIBRARIES = libdemon.la
noinst_HEADERS = Stochastic.h \
                        Ising.h \
                        Reservoir.h \
                        System.h \
                        Utilities.h \
                        ReservoirFactory.h \
			clangomp.h

bin_PROGRAMS = jdemon
jdemon_CXXFLAGS = $(GCOV_CFLAGS)\
		  $(OPENMP_CXXFLAGS)\
		  $(BOOST_CXXFLAGS)\
		  $(GSL_CXXFLAGS) 

jdemon_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS) \
		 $(GSL_LDFLAGS)

jdemon_LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
	       $(GSL_LIBS) \
	       $(GCOV_LIBS) \
	       libdemon.la

jdemon_SOURCES = main.cpp

libdemon_la_SOURCES = Stochastic.cpp \
		 	Ising.cpp \
		 	Reservoir.cpp \
			System.cpp \
		 	Utilities.cpp \
		 	ReservoirFactory.cpp

libdemon_la_CXXFLAGS = $(GCOV_CFLAGS)

libdemon_la_LIBADD = $(GSL_LIBS)
		     $(GCOV_LIBS)
