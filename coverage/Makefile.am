cov-reset:
	rm -rf *
	lcov --directory ${top_builddir}/test --zerocounters

all-local:
	lcov -b ${abs_top_srcdir} --capture --initial --directory ${abs_builddir}/src -o ${abs_builddir}/coverage/app_base.info

check-local:
	lcov -b ${abs_top_srcdir} --capture --directory . -t demoncore --output-file ${abs_builddir}/coverage/app_test.info

app_base.info: all
app_test.info: app_base.info check
app.info: app_base.info app_test.info
	lcov -a app_base.info -a app_test.info -o app.info

www/index.html: app.info
	genhtml -o www app.info

clean-local: cov-reset

coverage-local: www/index.html

.PHONY=coverage-local cov-reset
