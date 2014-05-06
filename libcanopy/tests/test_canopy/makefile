CFLAGS := --std=c89 --pedantic -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -I../../include -I$(LIBRED_DIR)/include

LIB_FLAGS := -L../.. -L$(LIBWEBSOCKETS_DIR)/lib -lcanopy -lwebsockets

SOURCE_FILES = test_canopy.c

debug:
	make -C ../.. debug
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) -o test_canopy

release:
	make -C ../.. release
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(RELEASE_FLAGS) -o test_canopy

run: debug
	./test_canopy

dbg: debug
	gdb ./test_canopy
clean:
	rm test_canopy

