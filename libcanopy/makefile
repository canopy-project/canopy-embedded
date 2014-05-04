#CFLAGS := --std=c89 --pedantic -Wall -Werror
CFLAGS := -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -Isrc -Iinclude -I/home/gregp/scratch/source/libwebsockets/lib -I/home/gregp/scratch/red/silver-c/include -I/home/gregp/scratch/red/silver-c/under_construction

SOURCE_FILES = src/haive.c src/haive_ws.c src/haive_sddl.c /home/gregp/scratch/red/silver-c/src/red_hash.c /home/gregp/scratch/red/silver-c/src/red_test.c /home/gregp/scratch/red/silver-c/src/red_json.c /home/gregp/scratch/red/silver-c/src/red_string.c


debug:
	gcc -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(DEBUG_FLAGS) -o libhaive.so

release:
	gcc -rdynamic -shared $(INCLUDE_FLAGS) $(SOURCE_FILES) $(RELEASE_FLAGS) -o libhaive.so

clean:
	rm libhaive.so
