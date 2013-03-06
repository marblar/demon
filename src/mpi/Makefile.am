bin_PROGRAMS = ${top_builddir}/jdemon-mpi

__top_builddir__jdemon_mpi_CXXFLAGS = $(GCOV_CFLAGS)\
                  $(OPENMP_CFLAGS)\
                  $(BOOST_CXXFLAGS)\
                  $(GSL_CFLAGS)\
		  -I${top_srcdir}/src/lib/

__top_builddir__jdemon_mpi_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)

__top_builddir__jdemon_mpi_LDADD = $(BOOST_PROGRAM_OPTIONS_LIBS) \
               $(GSL_LIBS) \
               $(GCOV_LIBS) \
	       ${top_builddir}/src/libdemon.la

__top_builddir__jdemon_mpi_SOURCES = main.cpp

