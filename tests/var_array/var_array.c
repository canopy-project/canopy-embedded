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
        CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_WS,
        CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_WS
    );

    result = canopy_var_init(canopy, "out bool[4] bool_ary");
    RedTest_Verify(test, "Init bool array", result == CANOPY_SUCCESS);

    result = canopy_var_init(canopy, "out string[6] string_ary");
    RedTest_Verify(test, "Init string array", result == CANOPY_SUCCESS);

    result = canopy_var_init(canopy, "out float64[6] float64_ary");
    RedTest_Verify(test, "Init float64 array", result == CANOPY_SUCCESS);

    result = canopy_var_set(canopy, "bool_ary", 
        CANOPY_VALUE_ARRAY(
            0, CANOPY_VALUE_BOOL(true), 
            1, CANOPY_VALUE_BOOL(false),
            2, CANOPY_VALUE_BOOL(true),
            3, CANOPY_VALUE_BOOL(false)
        )
    );
    RedTest_Verify(test, "Set bool array", result == CANOPY_SUCCESS);

    result = canopy_var_set(canopy, "float64_ary", 
        CANOPY_VALUE_ARRAY(
            3, CANOPY_VALUE_FLOAT64(143.04040)
        )
    );
    RedTest_Verify(test, "Set float array", result == CANOPY_SUCCESS);

    result = canopy_sync(canopy, NULL);
    RedTest_Verify(test, "Sync", result == CANOPY_SUCCESS);

    bool readBool1, readBool2;
    double readFloat64;

    result = canopy_var_get(canopy, "bool_ary",
            CANOPY_READ_ARRAY(
                1, CANOPY_READ_BOOL(&readBool1),
                2, CANOPY_READ_BOOL(&readBool2)));
    RedTest_Verify(test, "Bool[1] correct value", readBool1 == false);
    RedTest_Verify(test, "Bool[2] correct value", readBool2 == true);

    result = canopy_var_get(canopy, "float64_ary",
            CANOPY_READ_ARRAY(
                3, CANOPY_READ_FLOAT64(&readFloat64)));
    RedTest_Verify(test, "readFloat64 correct value", readFloat64 == 143.04040);

    result = canopy_shutdown_context(canopy);
    RedTest_Verify(test, "Shutdown", result == CANOPY_SUCCESS);

    return RedTest_End(test);
}
