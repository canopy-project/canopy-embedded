#include <canopy.h>
#include "red_test.h"
#include <stdio.h>

int main(int argc, const char *argv[])
{
    CanopyContext canopy;
    CanopyResultEnum result;
    RedTest test;

    test = RedTest_Begin(argv[0], NULL, NULL);

    canopy = canopy_init_context();
    canopy_debug_dump_opts(canopy);

    RedTest_Verify(test, "Canopy init", canopy);

    result = canopy_set_opt(canopy,
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f",
        CANOPY_SYNC_BLOCKING, true,
        CANOPY_SYNC_TIMEOUT_MS, 10000,
        CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_NOOP,
        CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_NOOP
    );

    RedTest_Verify(test, "Configure canopy options", result == CANOPY_SUCCESS);

    result = canopy_sync(canopy, NULL);

    RedTest_Verify(test, "Sync", result == CANOPY_SUCCESS);

    result = canopy_shutdown_context(canopy);
    RedTest_Verify(test, "Shutdown", result == CANOPY_SUCCESS);

    return RedTest_End(test);
}
