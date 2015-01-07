#include <canopy.h>
#include <stdio.h>

#define CHECK_RESULT(result) \
    if (result) \
    { \
        fprintf(stderr, "error (%d): %d\n", __LINE__, result); \
        return -1; \
    }

int main(void)
{
    CanopyResultEnum result;
    CanopyContext canopy;

    canopy = canopy_init_context();

    result = canopy_set_opt(canopy,
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f",
        CANOPY_SYNC_BLOCKING, true,
        CANOPY_SYNC_TIMEOUT_MS, 10000,
        CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_WS,
        CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_WS
    );
    CHECK_RESULT(result);

    // Initialize variables
    result = canopy_var_init(canopy, "out bool var_bool");
    CHECK_RESULT(result);

    result = canopy_var_init(canopy, "out int8 var_i8");
    CHECK_RESULT(result);
    
    result = canopy_var_init(canopy, "out uint8 var_u8");
    CHECK_RESULT(result);

    result = canopy_var_init(canopy, "out int16 var_i16");
    CHECK_RESULT(result);

    result = canopy_var_init(canopy, "out uint16 var_u16");
    CHECK_RESULT(result);

    result = canopy_var_init(canopy, "out int32 var_i32");
    CHECK_RESULT(result);

    result = canopy_var_init(canopy, "out uint32 var_u32");
    CHECK_RESULT(result);

    result = canopy_var_init(canopy, "out float32 var_f32");
    CHECK_RESULT(result);

    result = canopy_var_init(canopy, "out float64 var_f64");
    CHECK_RESULT(result);

    result = canopy_var_init(canopy, "out string var_string");
    CHECK_RESULT(result);

    // Set variables

    result = canopy_var_set(canopy, "var_bool", CANOPY_VALUE_BOOL(true));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_i8", CANOPY_VALUE_INT8(-15));
    CHECK_RESULT(result);
    
    result = canopy_var_set(canopy, "var_u8", CANOPY_VALUE_UINT8(15));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_i16", CANOPY_VALUE_INT16(-10000));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_u16", CANOPY_VALUE_UINT16(10000));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_i32", CANOPY_VALUE_INT32(-85000));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_u32", CANOPY_VALUE_UINT32(85000));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_f32", CANOPY_VALUE_FLOAT32(1.5f));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_f64", CANOPY_VALUE_FLOAT64(1.5000000001));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_string", CANOPY_VALUE_STRING("Hello World"));
    CHECK_RESULT(result);

    /*struct tm * dt;
    dt = gmtime(time());

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_datetime",
        CANOPY_DATETIME, dt
    );
    CHECK_RESULT(result);*/

    result = canopy_sync(canopy, NULL);
    CHECK_RESULT(result);

    // Set again to see second payload does not send configuration

    result = canopy_var_set(canopy, "var_u32", CANOPY_VALUE_UINT32(8500));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_f32", CANOPY_VALUE_FLOAT32(0.15f));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_f64", CANOPY_VALUE_FLOAT64(0.15000000001));
    CHECK_RESULT(result);

    result = canopy_var_set(canopy, "var_string", CANOPY_VALUE_STRING("Hello Again"));
    CHECK_RESULT(result);

    result = canopy_sync(canopy, NULL);
    CHECK_RESULT(result);

    // Sync w/ nothing dirty
    result = canopy_sync(canopy, NULL);
    CHECK_RESULT(result);

    result = canopy_shutdown_context(canopy);
    CHECK_RESULT(result);

    return (result == CANOPY_SUCCESS) ? 0 : -1;
}
