# This is a convenience library that implements the core functionality
# of the demon.
AM_CXXFLAGS=-O3 -g3

noinst_LTLIBRARIES = ${top_builddir}/src/libdemon.la

noinst_HEADERS = Stochastic.h \
                        Ising.h \
                        Reservoir.h \
                        System.h \
                        Utilities.h \
                        InstrumentFactories.h \
                        clangomp.h \
                        Measurement.h \
			cli.h \
			IsingGrid.h \
			RandomnessDelegate.h \
			CATools.h \
			TMGas.h

__top_builddir__src_libdemon_la_SOURCES = Stochastic.cpp \
                        Ising.cpp \
                        Reservoir.cpp \
                        System.cpp \
                        Utilities.cpp \
                        InstrumentFactories.cpp \
                        Measurement.cpp \
			cli.cpp \
			IsingGrid.cpp \
			CATools.cpp \
			TMGas.cpp

__top_builddir__src_libdemon_la_CPPFLAGS = $(BOOST_CPPFLAGS)

__top_builddir__src_libdemon_la_CXXFLAGS = $(GCOV_CFLAGS) \
			                        $(GSL_CFLAGS)

__top_builddir__src_libdemon_la_LIBADD = $(GSL_LIBS) \
						$(GCOV_LIBS) \
						$(BOOST_PROGRAM_OPTIONS_LIB)

check-targets: $(noinst_LTLIBRARIES)
