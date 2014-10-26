#include <canopy.h>

#define CHECK_RESULT(result) \
    if (!result) \
    { \
        fprintf(stderr, "error\n"); \
        return -1; \
    }

int main(void)
{
    CanopyResultEnum result;

    result = canopy_global_opt(
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f"
    );
    CHECK_RESULT(result);

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_i8",
        CANOPY_INT8, -15
    );
    CHECK_RESULT(result);
    
    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_u8",
        CANOPY_INT8, 15
    );
    CHECK_RESULT(result);

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_u16",
        CANOPY_UINT16, 40000
    );
    CHECK_RESULT(result);

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_u16",
        CANOPY_INT16, -10000
    );
    CHECK_RESULT(result);

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_u32",
        CANOPY_UINT32, 85000
    );
    CHECK_RESULT(result);

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_i32",
        CANOPY_INT32, -85000
    );
    CHECK_RESULT(result);

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_f32",
        CANOPY_FLOAT32, 1.5f
    );
    CHECK_RESULT(result);

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_f64",
        CANOPY_FLOAT32, 1.5000000001
    );
    CHECK_RESULT(result);

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_string",
        CANOPY_FLOAT32, "Hello World"
    );
    CHECK_RESULT(result);

    /*struct tm * dt;
    dt = gmtime(time());

    result = canopy_var_set(
        CANOPY_VAR_NAME, "var_datetime",
        CANOPY_DATETIME, dt
    );
    CHECK_RESULT(result);*/

    result = canopy_sync();
    CHECK_RESULT(result);

    return (result == CANOPY_SUCCESS) ? 0 : -1;
}
