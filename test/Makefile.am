if PERFORMANCE_TESTS_ENABLED
PERFORMANCE_DIR = performance
endif

SUBDIRS = $(PERFORMANCE_DIR) lib

TESTS = demoncore-wrapper.sh

EXTRA_DIST = demoncore-wrapper.sh

check-targets:
	make -C lib check-targets

if DEMON_GCOV_ENABLED
include coverage/Makemodule.am
endif

clean-local:
	rm -f *.xml
	rm -f *.log
