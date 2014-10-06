SOURCE_FILES := \
        example_post_sample.c

TARGET := ./example_post_sample

default: all

run:
	LD_LIBRARY_PATH=../../libcanopy $(TARGET)

$(TARGET) :
	gcc -I../../libcanopy/include $(SOURCE_FILES) -L../../libcanopy -lcanopy -lcurl -lwebsockets -Wall -Werror -g -o $(TARGET)

all: $(TARGET)
