all:
SOURCE_FILES := \
        simple_var_set.c

TARGET := build/simple_var_set

LIB_FLAGS := \
        -L$(CANOPY_EMBEDDED_ROOT)/build/_out/lib \
        -lred-canopy \
        -lcanopy \
        -lsddl \
        -lwebsockets-canopy \
        -lm \
        -lrt

INCLUDE_FLAGS := \
        -I$(CANOPY_EMBEDDED_ROOT)/build/_out/include

ifneq ($(CANOPY_CROSS_COMPILE),1)
    LIBS += curl
endif

default: all

run: $(TARGET)
	LD_LIBRARY_PATH=$(CANOPY_EMBEDDED_ROOT)/build/_out/lib $(TARGET)

dbg: $(TARGET)
	LD_LIBRARY_PATH=$(CANOPY_EMBEDDED_ROOT)/build/_out/lib gdb $(TARGET)

clean:
	rm -rf build



$(TARGET) : $(SOURCE_FILES)
	mkdir -p build
	$(CC) $(INCLUDE_FLAGS) $(SOURCE_FILES) $(LIB_FLAGS) $(CANOPY_CFLAGS) -o $(TARGET)

all: $(TARGET)
