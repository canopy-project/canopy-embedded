#CFLAGS := --std=c89 --pedantic -Wall -Werror
CFLAGS := -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

LIBRED_DIR := 3rdparty/libred

INCLUDE_FLAGS := -Isrc -Iinclude -I$(LIBRED_DIR)/include -I$(LIBRED_DIR)/under_construction

SOURCE_FILES = \
    src/canopy.c \
    src/cloudvar/st_cloudvar.c \
    src/http/st_http_curl.c \
    src/options/st_options.c \
    src/sync/st_sync.c \
    src/sddl.c \
    src/websocket/st_websocket.c \
    $(LIBRED_DIR)/src/red_hash.c \
    $(LIBRED_DIR)/src/red_test.c \
    $(LIBRED_DIR)/src/red_json.c \
    $(LIBRED_DIR)/src/red_string.c \
    $(LIBRED_DIR)/src/red_log.c \
    $(LIBRED_DIR)/src/red_uuid.c

debug:
	gcc -fPIC -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(DEBUG_FLAGS) -o libcanopy.so

release:
	gcc -fPIC -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(RELEASE_FLAGS) -o libcanopy.so

clean:
	rm libcanopy.so
