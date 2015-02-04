#include <canopy_os.h>
#include "log/st_log.h"
#include <stdbool.h>

int test_canopy_lib() {
    char *buf = canopy_os_calloc(1, 10);

    buf[0] = 'M';
    buf[1] = 'O';
    buf[2] = 'O';
    buf[3] = '!';
    buf[4] = '\0';
    canopy_os_log(buf);

    canopy_os_free(buf);    

    CANOPY_OS_ASSERT(0);



    st_log_init();
    st_log_debug("You should not see this\n");

    st_log_use_system_default(true);
    st_log_set_enabled(true);
    st_log_set_levels(ST_LOG_LEVEL_ALL);
    st_log_debug("Meow!\n");

    return 0;
}

