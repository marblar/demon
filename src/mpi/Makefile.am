bin_PROGRAMS = ${top_builddir}/jdemon-mpi

__top_builddir__jdemon_mpi_CXXFLAGS = $(GCOV_CFLAGS)\
                  $(OPENMP_CFLAGS)\
                  $(BOOST_CPPFLAGS)\
                  $(GSL_CFLAGS)\
		  -I${top_srcdir}/src/lib/

__top_builddir__jdemon_mpi_LDFLAGS = $(BOOST_LDFLAGS)

__top_builddir__jdemon_mpi_LDADD = $(BOOST_PROGRAM_OPTIONS_LIB) \
               $(GSL_LIBS) \
               $(GCOV_LIBS) \
		$(BOOST_MPI_LIB) \
		$(BOOST_SERIALIZATION_LIB) \
	       ${top_builddir}/src/libdemon.la

__top_builddir__jdemon_mpi_SOURCES = main.cpp

