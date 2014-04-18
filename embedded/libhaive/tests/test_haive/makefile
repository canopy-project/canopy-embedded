CFLAGS := --std=c89 --pedantic -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -I../../include -I/home/gregp/scratch/red/silver-c/include

LIB_FLAGS := -L../.. -lhaive

SOURCE_FILES = test_haive.c

debug:
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) -o test_haive

release:
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(RELEASE_FLAGS) -o test_haive

run: debug
	LD_LIBRARY_PATH=../.. ./test_haive

clean:
	rm test_haive

