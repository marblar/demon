SRC_BUILDDIR =  ${abs_top_builddir}/src
COVERAGE_DIR =  ${abs_builddir}
BUILDDIR =      ${abs_top_builddir}

cov-reset:
	rm -rf www
	rm *.info
	lcov --zerocounters -d ${abs_top_builddir} -q

#TODO: Dependencies!
base.info:
	lcov -c -i -d $(SRC_BUILDDIR) -o base.info

test.info:
	lcov -c -d $(SRC_BUILDDIR) -o test.info

src.info: test.info
	lcov -l base.info
	lcov -l test.info
	lcov -a base.info -a test.info -o src.info

# FIXME: This pattern blows.
app.info: src.info
	lcov -f --no-external -d /Users/mark/source/jdemon/src --extract src.info *src/* -o app.info

www/index.html: app.info
	genhtml -o www app.info

all-local: base.info
check-local: app.info
clean-local: cov-reset
coverage-local: all check www/index.html

.PHONY=coverage-local cov-reset
.INTERMEDIATE=
