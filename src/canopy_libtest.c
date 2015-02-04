#include <canopy_os.h>
#include "log/st_log.h"
#include "options/st_options.h"
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

    {
        STOptions opts = st_options_new_default();
        if (!opts) {
            st_log_error("Memory allocation failed");
            return -1;
        }

        if (st_option_is_set(opts, CANOPY_CLOUD_SERVER)) {
            st_log_info("Default cloud server: %s", opts->val_CANOPY_CLOUD_SERVER);
        } else {
            st_log_error("Cloud server not set");
        }
    }

    return 0;
}

