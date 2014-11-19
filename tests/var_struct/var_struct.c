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
    RedTest_Verify(test, "Canopy init", canopy);

    result = canopy_set_opt(canopy,
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f",
        CANOPY_SYNC_BLOCKING, true,
        CANOPY_SYNC_TIMEOUT_MS, 10000,
        CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_WS,
        CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_WS
    );

    result = canopy_var_init(canopy, "inout struct gps",
            CANOPY_INIT_FIELD("float32 latitude"),
            CANOPY_INIT_FIELD("float32 longitude"),
            CANOPY_INIT_FIELD("float32 altitude")
    );
    RedTest_Verify(test, "Init gps struct", result == CANOPY_SUCCESS);

    result = canopy_sync(canopy, NULL);
    RedTest_Verify(test, "sync", result == CANOPY_SUCCESS);

    result = canopy_var_set(canopy, "gps", 
        CANOPY_VALUE_STRUCT(
            "latitude", CANOPY_VALUE_FLOAT32(0.38838f),
            "longitude", CANOPY_VALUE_FLOAT32(0.494949f)
        )
    );

    result = canopy_sync(canopy, NULL);
    RedTest_Verify(test, "set latitude & longitude", result == CANOPY_SUCCESS);

    result = canopy_sync(canopy, NULL);
    RedTest_Verify(test, "sync", result == CANOPY_SUCCESS);

    result = canopy_shutdown_context(canopy);
    RedTest_Verify(test, "Shutdown", result == CANOPY_SUCCESS);

    return RedTest_End(test);
}
