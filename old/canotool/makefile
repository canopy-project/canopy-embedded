CFLAGS := --std=c99 --pedantic -Wall -Werror
DEBUG_FLAGS := $(CFLAGS) -g
RELEASE_FLAGS := $(CFLAGS) -O3

INCLUDE_FLAGS := -Ibuild -I../libcanopy/3rdparty/libred/include

LIB_FLAGS := -lcanopy -lwebsockets -lcurl

SOURCE_FILES = \
    src/cano.c \
    src/cano_test.c \
    src/cano_info.c \
    src/cano_provision.c \
    src/cano_gen.c \
    src/cano_uuid.c


HEADERIFY := tools/headerify/headerify

HEADER_TEMPLATES := resources/canopy_boilerplate_tail.h resources/canopy_boilerplate_head.h

HEADER_TEMPLATES_OUT := $(HEADER_TEMPLATES:resources/%.h=build/%.out.h)

debug: $(HEADER_TEMPLATES_OUT)
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) -o canotool

release: $(HEADER_TEMPLATES_OUT)
	gcc $(SOURCE_FILES) $(LIB_FLAGS) $(INCLUDE_FLAGS) $(RELEASE_FLAGS) -o canotool

run: debug
	./canotool

dbg: debug
	gdb ./canotool
clean:
	rm canotool build/*.h

build/canopy_boilerplate_tail.out.h: resources/canopy_boilerplate_tail.h
	$(HEADERIFY) _CANOPY_BOILERPLATE_TAIL < $< > $@

build/canopy_boilerplate_head.out.h: resources/canopy_boilerplate_head.h
	$(HEADERIFY) _CANOPY_BOILERPLATE_HEAD < $< > $@
