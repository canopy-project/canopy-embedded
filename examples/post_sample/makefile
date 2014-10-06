SOURCE_FILES := \
        example_post_sample.c

TARGET := build/example_post_sample

default: all

run: $(TARGET)
	LD_LIBRARY_PATH=../../libcanopy $(TARGET)

clean:
	rm -rf build


$(TARGET) :
	mkdir -p build
	gcc -I../../libcanopy/include $(SOURCE_FILES) -L../../libcanopy -lcanopy -lcurl -lwebsockets -Wall -Werror -g -o $(TARGET)

all: $(TARGET)
