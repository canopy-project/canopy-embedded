all:
SOURCE_FILES := \
        set_datatypes.c

TARGET := build/set_datatypes

default: all

run: $(TARGET)
	LD_LIBRARY_PATH=../../../$(CANOPY_EMBEDDED_ROOT)/build/_out/lib $(TARGET)

dbg: $(TARGET)
	LD_LIBRARY_PATH=../../../$(CANOPY_EMBEDDED_ROOT)/build/_out/lib gdb $(TARGET)

clean:
	rm -rf build


$(TARGET) : $(SOURCE_FILES)
	mkdir -p build
	gcc -I../../../3rdparty/libred/include -I../../include $(SOURCE_FILES) -L../../../$(CANOPY_EMBEDDED_ROOT)/build/_out/lib -lcanopy -lred-canopy -lcurl -lwebsockets -lm -Wall -Werror -g -o $(TARGET)

all: $(TARGET)
