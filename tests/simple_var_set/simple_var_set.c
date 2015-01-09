#include <canopy.h>
#include <red_test.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
    CanopyContext canopy;
    CanopyResultEnum result;
    RedTest test;

    test = RedTest_Begin(argv[0], NULL, NULL);

    if (!getenv("CANOPY_CLOUD_SERVER"))
    {
        return RedTest_Abort(test, "You must set CANOPY_CLOUD_SERVER env var");
    }

    if (!getenv("CANOPY_DEVICE_UUID"))
    {
        return RedTest_Abort(test, "You must set CANOPY_CLOUD_SERVER env var");
    }

    canopy = canopy_init_context();
    RedTest_Verify(test, "Canopy init", canopy);

    result = canopy_set_opt(canopy,
        CANOPY_CLOUD_SERVER, getenv("CANOPY_CLOUD_SERVER"),
        CANOPY_DEVICE_UUID, getenv("CANOPY_DEVICE_UUID"),
        CANOPY_SYNC_BLOCKING, true,
        CANOPY_SYNC_TIMEOUT_MS, 10000,
        CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_WS,
        CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_WS
    );
    RedTest_Verify(test, "Configure canopy options", result == CANOPY_SUCCESS);

    result = canopy_var_init(canopy, "inout float32 temperature");
    RedTest_Verify(test, "Init cloud variable", result == CANOPY_SUCCESS);

    result = canopy_var_set_float32(canopy, "temperature", 16.0f);
    RedTest_Verify(test, "Set cloud variable (local)", result == CANOPY_SUCCESS);

    result = canopy_sync(canopy, NULL);
    RedTest_Verify(test, "Sync", result == CANOPY_SUCCESS);

    result = canopy_shutdown_context(canopy);
    RedTest_Verify(test, "Shutdown", result == CANOPY_SUCCESS);

    return RedTest_End(test);
}
