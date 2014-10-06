CFLAGS := --std=gnu99 -Wall
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -I../3rdparty/source/ -I../3rdparty/source/Raspberry_Pi -I../../libcanopy/include -I$(LIBRED_DIR)/include

LIB_FLAGS := -L../../libcanopy -L$(LIBWEBSOCKETS_DIR)/lib -lcanopy -lwebsockets -lrt

SOURCE_FILES = canopy.example.temp_humidity_sensor.c \
    ../3rdparty/source/common_dht_read.c \
    ../3rdparty/source/Raspberry_Pi/pi_dht_read.c \
    ../3rdparty/source/Raspberry_Pi/pi_mmio.c

debug:
	#make -C ../../libcanopy debug
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) -o temp_humidity_sensor

release:
	#make -C ../../libcanopy release
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(RELEASE_FLAGS) -o temp_humidity_sensor

run: debug
	./temp_humidity_sensor

dbg: debug
	gdb ./temp_humidity_sensor
clean:
	rm temp_humidity_sensor
