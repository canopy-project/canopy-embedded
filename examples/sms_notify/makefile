SOURCE_FILES := \
        sms_notify.c

TARGET := build/notify_sms

default: all

run: $(TARGET)
	LD_LIBRARY_PATH=../../libcanopy $(TARGET)

dbg: $(TARGET)
	LD_LIBRARY_PATH=../../libcanopy gdb $(TARGET)

clean:
	rm -rf build


$(TARGET) :
	mkdir -p build
	gcc -I../../libcanopy/include $(SOURCE_FILES) -L../../libcanopy -lcanopy -lcurl -lwebsockets -Wall -Werror -g -o $(TARGET)

all: $(TARGET)
