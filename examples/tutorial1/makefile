CFLAGS := --std=c89 --pedantic -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -I../../libcanopy/include -I$(LIBRED_DIR)/include

LIB_FLAGS := -L../../libcanopy -L$(LIBWEBSOCKETS_DIR)/lib -lcanopy -lwebsockets

SOURCE_FILES = my_canopy_client.c

debug:
	make -C ../../libcanopy debug
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) -o my_canopy_client

release:
	make -C ../../libcanopy release
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(RELEASE_FLAGS) -o my_canopy_client

run: debug
	./my_canopy_client

dbg: debug
	gdb ./my_canopy_client
clean:
	rm my_canopy_client

