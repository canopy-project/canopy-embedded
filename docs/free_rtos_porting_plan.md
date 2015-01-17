FreeRTOS Porting Plan
-------------------------------------------------------------------------------

We need to port Canopy to FreeRTOS with some vendor-specific libraries.


libcanopy External Dependencies (Named by header file)
-------------------------------------------------------------------------------

Common C libaries

    assert.h
    string.h  - memcpy used in a few places
    stdarg.h  - Many canopy_ routines take variable arguments.
    time.h    - clock_gettime(CLOCK_MONOTONIC)
    stdlib.h  - malloc, calloc
    stdio.h   - printf, fprintf, snprintf
    stdbool.h - bool datatype
    stdint.h -  int datatypes

    PORTING EFFORT REQUIRED: TBD

Third party libraries

    curl/curl.h     - Used by "libcanopy/src/http" module.  Currently the ARM
                      builds skip the libcurl dependency, but that is a hack.
    libwebsockets.h - Used by "libcanopy/src/ws" module
    
    PORTING EFFORT REQUIRED:
    We might need to use vendor-specific alternatives for these, and first
    building an abstraction layer.  Perhaps libcanopy/src/http and
    libcanopy/src/ws can act as those abstraction layers?


Other Canopy libraries

    sddl.h       - For parsing SDDL format
    red_json.h   - For encoding/decoding JSON payloads
    red_string.h - Only RedString_Strdup and RedString_PrintToChars
    red_hash.h   - For various key:value mappings.  Maybe too mem/cpu intensive?
    red_log.h    - For logging, but not used consistently
    red_test.h   - Used for reporting by unit tests

    PORTING EFFORT REQUIRED: TBD


libsddl External Dependencies (Named by header file)
-------------------------------------------------------------------------------

Common C libraries

    assert.h
    stdbool.h - bool datatype
    stdint.h  - int datatypes
    stdio.h   - printf used (inappropriately?) for logging
    stdlib.h  - malloc, calloc
    string.h  - strcmp used in a few places

    PORTING EFFORT REQUIRED: 
    Same effort as for libcanopy.  The only addition is "strcmp".

Third party libraries

    None

Other Canopy libraries

    red_string.h - Used a bunch for string manipulation
    red_json.h   - Encoding/decoding of JSON
    red_test.h   - Used for reporting by unit tests

    PORTING EFFORT REQUIRED: 
    Same effort as for libcanopy.  The only addition is some more RedString_
    functions are used here.



libred External Dependencies (Named by header file)
-------------------------------------------------------------------------------

Common C libraries

    stdio.h
    stdlib.h
    stdbool.h
    string.h
    assert.h
    stdint.h
    stdint.h
    sys/time.h
    ctype.h
    math.h
    stdarg.h

    PORTING EFFORT REQUIRED: 
    TBD.  Some overlap with libcanopy.

Third party libraries

    None

Other Canopy libraries

    None

Other Notes

    The "red_hash" module is used a lot.  The implementation is not very space
    efficient and does a lot of memory allocation.  Not sure if the memory/cpu
    usage will be too high for certain target platforms.


libwebsocket External Dependencies
-------------------------------------------------------------------------------
    openssl
    zlib
    Others?

    PORTING EFFORT REQUIRED: 
    TBD: We need to figure out if we port libwebsockets ourselves, or if the
    chip vendor has provided an appropriate alternative.

libcurl External Dependencies
-------------------------------------------------------------------------------
    openssl
    Others?

    PORTING EFFORT REQUIRED: 
    TBD: We need to figure out if we port libcurl ourselves, or if the chip
    vendor has provided an appropriate alternative.
