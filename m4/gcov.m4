# https://github.com/kr/cubby/blob/master/m4/gcov.m4
dnl Profide a configure option to enable gcov compiler flags.
dnl Derived from gcov.m4 in the Gobject-Introspection repository.
AC_DEFUN([DEMON_ENABLE_GCOV],
[
  AC_ARG_ENABLE(gcov,
    AC_HELP_STRING([--enable-gcov],
      [compile with coverage profiling instrumentation (gcc only)]),
    enable_gcov=$enableval,
    enable_gcov=no)
  if test x$enable_gcov = xyes
  then
    AS_COMPILER_FLAGS(GCOV_CFLAGS, "-fprofile-arcs -ftest-coverage")
    dnl libtool 1.5.22 and lower strip -fprofile-arcs from the flags
    dnl passed to the linker, which is a bug; -fprofile-arcs implicitly
    dnl links in -lgcov, so we do it explicitly here for the same effect
    GCOV_LIBS=-lgcov
    AC_SUBST(GCOV_CFLAGS)
    AC_SUBST(GCOV_LIBS)
    GCOV=gcov
    AC_SUBST(GCOV)

    DEMON_GCOV_ENABLED=yes
    AC_DEFINE_UNQUOTED(DEMON_GCOV_ENABLED, 1,
      [Defined if gcov is enabled to force a rebuild due to config.h changing])

    # Force the user to turn off optimization
    AC_MSG_NOTICE([gcov enabled, adding "-g -O0" to CFLAGS])
    AS_COMPILER_FLAGS(CXXFLAGS, "-g -O0")
  fi
  AM_CONDITIONAL(DEMON_GCOV_ENABLED, test x$enable_gcov = xyes)
])
