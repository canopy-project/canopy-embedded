# Copyright 2014-2015 SimpleThings, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This is the makefile for libcanopy.
#
# It is affected by the following environment variables:
#
#   CANOPY_CFLAGS
#       Desired compilation flags.  Defaults to "".
#
#   CC
#       Compiler to use, such as "gcc".
#
LIBRED_DIR := ../3rdparty/libred
LIBSDDL_DIR := ../libsddl
LIBWEBSOCKETS_DIR := ../3rdparty/libwebsockets
CANOPY_EDK_BUILD_NAME ?= default
CANOPY_EDK_BUILD_OUTDIR ?= _out/$(CANOPY_EDK_BUILD_NAME)

INCLUDE_FLAGS := \
	-Isrc \
	-Iinclude \
	-I$(LIBSDDL_DIR)/include \
	-I$(LIBRED_DIR)/include \
	-I$(LIBRED_DIR)/under_construction \
	-I$(LIBWEBSOCKETS_DIR)/lib

SOURCE_FILES := \
    src/canopy.c \
    src/cloudvar/st_cloudvar.c \
    src/cloudvar/st_cloudvar_common.c \
    src/cloudvar/st_cloudvar_basic.c \
    src/cloudvar/st_cloudvar_array.c \
    src/cloudvar/st_cloudvar_struct.c \
    src/cloudvar/st_cloudvar_system.c \
    src/log/st_log.c \
    src/options/st_options.c \
    src/sync/st_sync.c \
    src/websocket/st_websocket.c

# Hack: For now, remove curl dependency if cross compiling
ifeq ($(CANOPY_CROSS_COMPILE),1)
    SOURCE_FILES += src/http/st_http_stubs.c
else
    SOURCE_FILES += src/http/st_http_curl.c
endif

.PHONY: default
default:
	mkdir -p $(CANOPY_EDK_BUILD_OUTDIR)
	$(CC) -fPIC -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(CANOPY_CFLAGS) -o $(CANOPY_EDK_BUILD_OUTDIR)/libcanopy.so

.PHONY: clean
clean:
	rm -rf $(CANOPY_EDK_BUILD_OUTDIR)
