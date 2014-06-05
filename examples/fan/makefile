CFLAGS := --std=c89 --pedantic -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -I../../libcanopy/include -I$(LIBRED_DIR)/include

LIB_FLAGS := -L../../libcanopy -L$(LIBWEBSOCKETS_DIR)/lib -lcanopy -lwebsockets

SOURCE_FILES = canopy.example.fan.c

CANOGEN = ../../canogen/canogen

debug:
	make -C ../../libcanopy debug
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) -o fan

release:
	make -C ../../libcanopy release
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(RELEASE_FLAGS) -o fan

run: debug
	./fan

dbg: debug
	gdb ./fan
clean:
	rm fan

.PHONY: canogen
canogen:
	make -C ../../canogen
	$(CANOGEN) fan.sddl canopy.example.fan
