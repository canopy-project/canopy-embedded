SOURCE_FILES := \
        example_control_cb.c

TARGET := build/example_control_cb

default: all

run: $(TARGET)
	LD_LIBRARY_PATH=../../libcanopy $(TARGET)

dbg: $(TARGET)
	LD_LIBRARY_PATH=../../libcanopy gdb $(TARGET)

clean:
	rm -rf build


$(TARGET) : $(SOURCE_FILES)
	mkdir -p build
	gcc -I../../libcanopy/include $(SOURCE_FILES) -L../../libcanopy -lcanopy -lcurl -lwebsockets -Wall -Werror -g -o $(TARGET)

all: $(TARGET)
