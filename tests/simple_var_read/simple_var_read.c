#include <canopy.h>
#include "red_test.h"
#include <stdio.h>

// Just tests local copy of cloud variable.  Doesn't "sync" w/ server.
int main(int argc, const char *argv[])
{
    CanopyContext canopy;
    CanopyResultEnum result;
    RedTest test;
    float temperature;

    test = RedTest_Begin(argv[0], NULL, NULL);

    canopy = canopy_init_context();
    RedTest_Verify(test, "Canopy init", canopy);

    result = canopy_var_init(canopy, "inout float32 temperature");
    RedTest_Verify(test, "Initialize cloud var", result == CANOPY_SUCCESS);

    result = canopy_set_opt(canopy,
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f",
        CANOPY_SYNC_BLOCKING, true,
        CANOPY_SYNC_TIMEOUT_MS, 10000,
        CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_NOOP,
        CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_NOOP
    );
    RedTest_Verify(test, "Configure canopy options", result == CANOPY_SUCCESS);

    result = canopy_var_set_float32(canopy, "temperature", 18.5f);
    RedTest_Verify(test, "Set cloud variable (local)", result == CANOPY_SUCCESS);

    result = canopy_var_get_float32(canopy, "temperature", &temperature);
    RedTest_Verify(test, "Get cloud variable (local)", result == CANOPY_SUCCESS);

    RedTest_Verify(test, "Temperature matches", temperature == 18.5f);

    result = canopy_shutdown_context(canopy);
    RedTest_Verify(test, "Shutdown", result == CANOPY_SUCCESS);

    return RedTest_End(test);
}
