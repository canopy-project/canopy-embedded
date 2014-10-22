all:
SOURCE_FILES := \
        simple_var_read.c

TARGET := build/simple_var_read

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
