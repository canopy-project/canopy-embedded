CFLAGS := --std=gnu99 --pedantic -Wall
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -I../../libcanopy/include -I$(LIBRED_DIR)/include -I../3rdparty/source/Raspberry_Pi

LIB_FLAGS := -L../../libcanopy -L$(LIBWEBSOCKETS_DIR)/lib -lcanopy -lwebsockets -lrt

SOURCE_FILES = canopy.example.fan.c \
    ../3rdparty/source/common_dht_read.c \
    ../3rdparty/source/Raspberry_Pi/pi_dht_read.c \
    ../3rdparty/source/Raspberry_Pi/pi_mmio.c

CANOGEN = ../../canogen/canogen

debug:
	#make -C ../../libcanopy debug
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) -o fan

release:
	#make -C ../../libcanopy release
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
