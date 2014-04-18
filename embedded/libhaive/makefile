CFLAGS := --std=c89 --pedantic -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -Iinclude -I/home/gregp/scratch/red/silver-c/include

SOURCE_FILES = src/haive.c /home/gregp/scratch/red/silver-c/src/red_hash.c

debug:
	gcc --shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(DEBUG_FLAGS) -o libhaive

release:
	gcc --shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(RELEASE_FLAGS) -o libhaive

clean:
	rm libhaive
