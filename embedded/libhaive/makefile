#CFLAGS := --std=c89 --pedantic -Wall -Werror
CFLAGS := -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -Iinclude -I/home/gregp/scratch/source/libwebsockets/lib -I/home/gregp/scratch/red/silver-c/include

SOURCE_FILES = src/haive.c src/haive_ws.c /home/gregp/scratch/red/silver-c/src/red_hash.c /home/gregp/scratch/red/silver-c/src/red_test.c

debug:
	gcc -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(DEBUG_FLAGS) -o libhaive.so

release:
	gcc -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(RELEASE_FLAGS) -o libhaive.so

clean:
	rm libhaive.so
