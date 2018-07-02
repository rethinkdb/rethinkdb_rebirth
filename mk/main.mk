# Copyright 2010-2013 RethinkDB, all rights reserved.

# This is the main file that controls the rethinkdb build process.
# It is run by the top-level Makefile
#
# The actual build rules are located in mk/*.mk and **/build.mk

# The default goal
.PHONY: default-goal
default-goal: real-default-goal

# check-env.mk provides check-env-start and check-env-check
include $(TOP)/mk/check-env.mk

# Include custom.mk and the config file
include $(TOP)/mk/configure.mk

# Makefile related definitions
include $(TOP)/mk/lib.mk

# The default goal
real-default-goal: $(DEFAULT_GOAL)

# Paths, build rules, packaging, ...
include $(TOP)/mk/paths.mk

# Download and build internal tools like v8 and gperf
include $(TOP)/mk/support/build.mk

# make install
include $(TOP)/mk/install.mk

ifeq (Windows,$(OS))

# Windows build
include $(TOP)/mk/windows.mk

else # Windows

# Building the rethinkdb executable
include $(TOP)/src/build.mk

# Packaging for deb, osx, ...
include $(TOP)/mk/packaging.mk

# Rules for tools like valgrind and code coverage report
include $(TOP)/mk/tools.mk

endif # Windows

.PHONY: clean
clean: build-clean

.PHONY: all
ifeq (Windows,$(OS))
  all: windows-all
else
  # Build the executable
  all: $(TOP)/src/all
endif

.PHONY: generate
generate: generate-headers

.PHONY: test
test: $(BUILD_DIR)/rethinkdb $(BUILD_DIR)/rethinkdb-unittest
	$P RUN-TESTS
	# MAKEFLAGS= $(TOP)/test/run -b $(BUILD_DIR)
