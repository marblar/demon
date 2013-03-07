if PERFORMANCE_TESTS_ENABLED
PERFORMANCE_DIR = performance
endif

SUBDIRS = $(PERFORMANCE_DIR) lib

TESTS = demoncore-wrapper.sh

EXTRA_DIST = demoncore-wrapper.sh

clean-local:
	rm -f *.xml
