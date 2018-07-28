# Copyright 2018-present RebirthDB
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use
# this file except in compliance with the License. You may obtain a copy of the
# License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed
# under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#
# This file incorporates work covered by the following copyright:
#
#     Copyright 2010-present, The Linux Foundation, portions copyright Google and
#     others and used with permission or subject to their respective license
#     agreements.
#
#     Licensed under the Apache License, Version 2.0 (the "License");
#     you may not use this file except in compliance with the License.
#     You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#     Unless required by applicable law or agreed to in writing, software
#     distributed under the License is distributed on an "AS IS" BASIS,
#     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#     See the License for the specific language governing permissions and
#     limitations under the License.

##### Generate ctags or etags file

CTAGSPROG ?= ctags
ETAGSPROG ?= etags

TAGSFILE  ?= $(TOP)/src/.tags
ETAGSFILE ?= $(TOP)/src/TAGS

CTAGFLAGS ?= -R --c++-kinds=+p --fields=+iaS --extra=+q --langmap="c++:.cc.tcc.hpp"

.PHONY: tags etags
tags: $(TAGSFILE)
etags: $(ETAGSFILE)

$(TAGSFILE): FORCE
	$P TAGS $@
	$(CTAGSPROG) $(CTAGFLAGS) -f $@ $(TOP)/src

$(ETAGSFILE): FORCE
	$P ETAGS $@
	rm -rf $@
	touch $@
	find $(TOP)/src \( -name \*.hpp -or -name \*.cc -or -name \*.tcc \) -print0 \
	  | xargs -0 $(ETAGSPROG) -l c++ -a -o $@

##### cscope

CSCOPE_XREF ?= $(TOP)/src/.cscope

.PHONY: cscope
cscope:
	$P CSCOPE
	cscope -bR -f $(CSCOPE_XREF)

##### Coverage report

ifeq (1,$(COVERAGE))
  .PHONY: coverage
  coverage: $(BUILD_DIR)/$(SERVER_UNIT_TEST_NAME)
	$P RUN $<
	$(BUILD_DIR)/$(SERVER_UNIT_TEST_NAME) --gtest_filter=$(UNIT_TEST_FILTER)
	$P LCOV $(BUILD_DIR)/coverage.full.info
	lcov --directory $(OBJ_DIR) --base-directory "`pwd`" --capture --output-file $(BUILD_DIR)/coverage.full.info
	$P LCOV $(BUILD_DIR)/converage.info
	lcov --remove $(BUILD_DIR)/coverage.full.info /usr/\* -o $(BUILD_DIR)/coverage.info
	$P GENHTML $(BUILD_DIR)/coverage
	genhtml --demangle-cpp --no-branch-coverage --no-prefix -o $(BUILD_DIR)/coverage $(BUILD_DIR)/coverage.info
	echo "Wrote unit tests coverage report to $(BUILD_DIR)/coverage"
endif

##### Code information

analyze: $(SOURCES)
	$P CLANG-ANALYZE
	clang --analyze $(RT_CXXFLAGS) $(SOURCES)

showdefines:
	$P SHOW-DEFINES ""
	$(RT_CXX) $(RT_CXXFLAGS) -m32 -E -dM - < /dev/null
