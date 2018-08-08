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

# This makefile defines paths that are needed by the other Makefiles

BUILD_ROOT_DIR := $(TOP)/build
PACKAGING_DIR := $(TOP)/packaging
PACKAGES_DIR := $(BUILD_ROOT_DIR)/packages
SUPPORT_SRC_DIR := $(TOP)/external
SUPPORT_BUILD_DIR := $(BUILD_ROOT_DIR)/external
SUPPORT_LOG_DIR := $(SUPPORT_BUILD_DIR)

# If the BUILD_DIR is not set, generate a name that depends on the different settings
ifeq ($(BUILD_DIR),)
  ifeq ($(DEBUG),1)
    BUILD_DIR := $(BUILD_ROOT_DIR)/debug
  else
    BUILD_DIR := $(BUILD_ROOT_DIR)/release
  endif

  ifeq ($(COMPILER), CLANG)
    BUILD_DIR += clang
  else ifeq ($(COMPILER), INTEL)
      BUILD_DIR += intel
  endif

  ifeq (1,$(LEGACY_LINUX))
    BUILD_DIR += legacy
  endif

  ifeq (1,$(LEGACY_GCC))
    BUILD_DIR += legacy-gcc
  endif

  ifeq (1,$(NO_EVENTFD))
    BUILD_DIR += noeventfd
  endif

  ifeq (1,$(NO_EPOLL))
    BUILD_DIR += noepoll
  endif

  ifeq (1,$(VALGRIND))
    BUILD_DIR += valgrind
  endif

  ifeq (1,$(THREADED_COROUTINES))
    BUILD_DIR += threaded
  endif

  ifeq (1,$(CORO_PROFILING))
    BUILD_DIR += coro-prof
  endif

  ifneq ($(DEFAULT_ALLOCATOR),$(ALLOCATOR))
    BUILD_DIR += $(ALLOCATOR)
  endif

  BUILD_DIR := $(subst $(space),_,$(BUILD_DIR))
endif
BUILD_DIR_ABS = $(realpath $(BUILD_DIR))

GDB_FUNCTIONS_NAME := rebirthdb-gdb.py

PACKAGE_NAME := $(VANILLA_PACKAGE_NAME)
SERVER_UNIT_TEST_NAME := $(SERVER_EXEC_NAME)-unittest

PROTO_FILE_SRC := $(TOP)/src/rdb_protocol/ql2.proto
PROTO_DIR := $(BUILD_ROOT_DIR)/proto

DEP_DIR := $(BUILD_DIR)/dep
OBJ_DIR := $(BUILD_DIR)/obj

##### To rebuild when Makefiles change

ifeq ($(IGNORE_MAKEFILE_CHANGES),1)
  MAKEFILE_DEPENDENCY :=
else
  MAKEFILE_DEPENDENCY = $(filter %Makefile,$(MAKEFILE_LIST)) $(filter %.mk,$(MAKEFILE_LIST))
endif
