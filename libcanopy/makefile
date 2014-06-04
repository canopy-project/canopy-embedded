#CFLAGS := --std=c89 --pedantic -Wall -Werror
CFLAGS := -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -Isrc -Iinclude -I$(LIBWEBSOCKETS_DIR)/../lib -I$(LIBRED_DIR)/include -I$(LIBRED_DIR)/under_construction

SOURCE_FILES = src/canopy.c src/canopy_event.c src/canopy_ws.c src/sddl.c $(LIBRED_DIR)/src/red_hash.c $(LIBRED_DIR)/src/red_test.c $(LIBRED_DIR)/src/red_json.c $(LIBRED_DIR)/src/red_string.c $(LIBRED_DIR)/src/red_log.c

debug:
	gcc -fPIC -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(DEBUG_FLAGS) -o libcanopy.so

release:
	gcc -fPIC -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(RELEASE_FLAGS) -o libcanopy.so

clean:
	rm libcanopy.so
