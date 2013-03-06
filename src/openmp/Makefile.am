# A shared-memory frontend for jdemon-omp. Kept around mostly for posterity.

bin_PROGRAMS = ${top_builddir}/jdemon-omp

__top_builddir__jdemon_omp_CXXFLAGS = $(GCOV_CFLAGS)\
                  $(OPENMP_CFLAGS)\
                  $(BOOST_CPPFLAGS)\
                  $(GSL_CFLAGS) \
		  -I${top_srcdir}/src/lib/

__top_builddir__jdemon_omp_LDFLAGS = $(BOOST_LDFLAGS)

__top_builddir__jdemon_omp_LDADD = $(BOOST_PROGRAM_OPTIONS_LIB) \
               $(GSL_LIBS) \
               $(GCOV_LIBS) \
               ${top_builddir}/src/libdemon.la

__top_builddir__jdemon_omp_SOURCES = main.cpp
