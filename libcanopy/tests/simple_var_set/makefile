all:
SOURCE_FILES := \
        simple_var_set.c

TARGET := build/simple_var_set

default: all

run: $(TARGET)
	LD_LIBRARY_PATH=../.. $(TARGET)

dbg: $(TARGET)
	LD_LIBRARY_PATH=../.. gdb $(TARGET)

clean:
	rm -rf build


$(TARGET) : $(SOURCE_FILES)
	mkdir -p build
	gcc -I../../3rdparty/libred/include -I../../include $(SOURCE_FILES) -L../.. -lcanopy -lcurl -lwebsockets -Wall -Werror -g -o $(TARGET)

all: $(TARGET)
