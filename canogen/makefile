CFLAGS := --std=c89 --pedantic -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -Ibuild -I../libcanopy/include -I$(LIBRED_DIR)/include

LIB_FLAGS := -L../libcanopy -L$(LIBWEBSOCKETS_DIR)/lib -lcanopy -lwebsockets

SOURCE_FILES = src/canogen.c

HEADERIFY := tools/headerify/headerify

HEADER_TEMPLATES := resources/canopy_boilerplate_head.h

HEADER_TEMPLATES_OUT := $(HEADER_TEMPLATES:resources/%.h=build/%.out.h)

debug: $(HEADER_TEMPLATES_OUT)
	make -C ../libcanopy debug
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) -o canogen

release: $(HEADER_TEMPLATES_OUT)
	make -C ../libcanopy release
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(RELEASE_FLAGS) -o canogen

run: debug
	./canogen

dbg: debug
	gdb ./canogen
clean:
	rm canogen build/*.h

build/%.out.h : resources/%.h
	$(HEADERIFY) _CANOPY_BOILERPLATE_HEAD < $< > $@

