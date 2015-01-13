ifneq ($(CANOPY_EDK_ENVSETUP),1)
    $(error You must first run "source envsetup.sh" from the /build directory)
endif

SOURCE_FILES := \
        simple_var_set.c

TARGET := $(CANOPY_EDK_BUILD_OUTDIR)/simple_var_set

LIB_FLAGS := \
        -L$(CANOPY_EDK_BUILD_DESTDIR)/lib \
        -lred-canopy \
        -lcanopy \
        -lsddl \
        -lwebsockets-canopy \
        -lm \
        -lrt

INCLUDE_FLAGS := \
        -I$(CANOPY_EDK_BUILD_DESTDIR)/include

ifneq ($(CANOPY_CROSS_COMPILE),1)
    LIB_FLAGS += -lcurl
endif

default: all

run: $(TARGET)
	$(TARGET)

dbg: $(TARGET)
	gdb $(TARGET)

clean:
	rm -rf $(CANOPY_EDK_BUILD_OUTDIR)

$(TARGET) : $(SOURCE_FILES)
	mkdir -p $(CANOPY_EDK_BUILD_OUTDIR)
	$(CC) $(INCLUDE_FLAGS) $(SOURCE_FILES) $(LIB_FLAGS) $(CANOPY_CFLAGS) -o $(TARGET)

all: $(TARGET)
