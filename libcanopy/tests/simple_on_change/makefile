all:
SOURCE_FILES := \
        simple_on_change.c

TARGET := build/simple_on_change

default: all

run: $(TARGET)
	LD_LIBRARY_PATH=../.. $(TARGET)

dbg: $(TARGET)
	LD_LIBRARY_PATH=../.. gdb $(TARGET)

clean:
	rm -rf build


$(TARGET) : $(SOURCE_FILES)
	mkdir -p build
	gcc -I../../include $(SOURCE_FILES) -L../.. -lcanopy -lcurl -lwebsockets -Wall -Werror -g -o $(TARGET)

all: $(TARGET)
