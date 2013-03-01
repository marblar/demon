TESTS = demoncore
check_PROGRAMS = $(TESTS)
demoncore_SOURCES = core.cpp\
			SystemTest.cpp

demoncore_CXXFLAGS = -I${top_srcdir}/src \
			$(CPPUNIT_CFLAGS) \
			$(GCOV_CFLAGS)

demoncore_LDFLAGS = $(CPPUNIT_LIBS) \
			${top_builddir}/src/libdemon.la

demoncore_LIBS = $(GCOV_LIBS) \
			$(CPPUNIT_LIBS)

noinst_HEADERS = SystemTest.h

if DEMON_GCOV_ENABLED
cov-reset:
	@rm -rf coverage
	@find . -name "*.gcda" -exec rm {};
	@lcov --directory . --zerocounters

coverage/app_base.info: all
	@mkdir -p coverage
	@lcov --no-external --compat-libtool --capture --initial \
		--directory ${top_builddir}/src -o coverage/app_base.info -b ${top_srcdir}

coverage/app_test.info: coverage/app_base.info check
	@lcov --no-external --compat-libtool --capture --directory ${top_builddir}/test\
		 -t demoncore --output-file coverage/app_test.info -b ${top_srcdir}


coverage/app.info: coverage/app_base.info coverage/app_test.info
	@lcov -a coverage/app_base.info -a coverage/app_test.info -o coverage/app.info

coverage/index.html: coverage/app.info
	@genhtml -o coverage/ coverage/app.info

clean-local: cov-reset

coverage: coverage/index.html

.PHONY=coverage cov-reset
endif
