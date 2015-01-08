LIBRED_DIR := ../3rdparty/libred
LIBSDDL_DIR := ../libsddl
LIBWEBSOCKETS_DIR := ../3rdparty/libwebsockets

INCLUDE_FLAGS := \
	-Isrc \
	-Iinclude \
	-I$(LIBSDDL_DIR)/include \
	-I$(LIBRED_DIR)/include \
	-I$(LIBRED_DIR)/under_construction \
	-I$(LIBWEBSOCKETS_DIR)/lib

SOURCE_FILES = \
    src/canopy.c \
    src/cloudvar/st_cloudvar.c \
    src/cloudvar/st_cloudvar_common.c \
    src/cloudvar/st_cloudvar_basic.c \
    src/cloudvar/st_cloudvar_array.c \
    src/cloudvar/st_cloudvar_struct.c \
    src/cloudvar/st_cloudvar_system.c \
    src/http/st_http_curl.c \
    src/log/st_log.c \
    src/options/st_options.c \
    src/sync/st_sync.c \
    src/websocket/st_websocket.c

.PHONY: default
default:
	$(CC) -fPIC -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(CANOPY_CFLAGS) -o libcanopy.so

.PHONY: clean
clean:
	rm libcanopy.so
