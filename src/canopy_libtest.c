#include <canopy_os.h>

int test_canopy_lib() {
    char *buf = canopy_os_calloc(1, 10);

    buf[0] = 'M';
    buf[1] = 'O';
    buf[2] = 'O';
    buf[3] = '!';
    buf[4] = '\0';
    canopy_os_log(buf);

    canopy_os_free(buf);    

    canopy_os_assert(0);

    return 0;
}

