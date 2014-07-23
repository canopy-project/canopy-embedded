TARGETS := fan 
.PHONY: default
default: $(TARGETS)

SOURCE_FILES = fan.c \
    ../3rdparty/source/common_dht_read.c \
    ../3rdparty/source/Raspberry_Pi/pi_dht_read.c \
    ../3rdparty/source/Raspberry_Pi/pi_mmio.c

INCLUDE_FLAGS := -I../3rdparty/source/Raspberry_Pi

fan: $(SOURCE_FILES)
	gcc $(SOURCE_FILES) -std=gnu99 -g $(INCLUDE_FLAGS) -lcanopy -lcurl -lwebsockets -lrt -o $@
