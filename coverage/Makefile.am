SRC_BUILDDIR =  ${abs_top_builddir}/src
COVERAGE_DIR =  ${abs_builddir}
BUILDDIR =      ${abs_top_builddir}
BRANCH_FLAGS =  --rc lcov_branch_coverage=1 


cov-reset:
	rm -rf www
	rm *.info
	lcov --zerocounters -d ${abs_top_builddir} -q

#TODO: Dependencies!
base.info:
	lcov $(BRANCH_FLAGS) -c -i -d $(SRC_BUILDDIR) -o base.info

test.info:
	lcov $(BRANCH_FLAGS) -c -d $(SRC_BUILDDIR) -o test.info

src.info: test.info
	lcov -l base.info
	lcov -l test.info
	lcov $(BRANCH_FLAGS) -a base.info -a test.info -o src.info

# FIXME: This pattern blows.
app.info: src.info
	lcov $(BRANCH_FLAGS) -f --no-external -d /Users/mark/source/jdemon/src --extract src.info *src/* -o app.info

www/index.html: app.info
	genhtml $(BRANCH_FLAGS) -t jdemon -o www app.info

all-local: base.info
check-local: app.info www/index.html
clean-local: cov-reset 

.PHONY=coverage-local cov-reset
