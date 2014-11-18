#include <canopy.h>
#include <red_test.h>
#include <stdio.h>

// Just tests local copy of cloud variable.  Doesn't "sync" w/ server.
int main(int argc, const char *argv[])
{
    CanopyContext canopy;
    CanopyResultEnum result;
    RedTest test;

    test = RedTest_Begin(argv[0], NULL, NULL);

    canopy = canopy_init_context();
    RedTest_Verify(test, "Canopy init", canopy);

    result = canopy_set_opt(canopy,
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f",
        CANOPY_SYNC_BLOCKING, true,
        CANOPY_SYNC_TIMEOUT_MS, 10000,
        CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_WS,
        CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_WS
    );
    RedTest_Verify(test, "Configure canopy options", result == CANOPY_SUCCESS);

    result = canopy_var_config(canopy, "dimlevel", 
            CANOPY_VAR_DATATYPE, CANOPY_DATATYPE_INT16,
            CANOPY_VAR_DIRECTION, CANOPY_DIRECTION_IN
    );
    RedTest_Verify(test, "Configure variable \"dimlevel\"", result == CANOPY_SUCCESS);

    result = canopy_sync(canopy, NULL);

    result = canopy_shutdown_context(canopy);
    RedTest_Verify(test, "Shutdown", result == CANOPY_SUCCESS);

    return RedTest_End(test);
}
