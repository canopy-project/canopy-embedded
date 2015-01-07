#include <canopy.h>
#include "red_test.h"
#include <stdio.h>
#include <string.h>

static bool callbackTriggered;

static int handle_dimmer_level(CanopyContext ctx, const char *varName, void *extra)
{
    RedTest test = (RedTest)extra;
    float value;
    CanopyResultEnum result;

    RedTest_Verify(test, "OnChange: varName matches", !strcmp(varName, "dimmer_level"));

    result = canopy_var_get(ctx, varName, CANOPY_READ_FLOAT32(&value));
    RedTest_Verify(test, "OnChange: canopy_var_get", result == CANOPY_SUCCESS);
    RedTest_Verify(test, "OnChange: value", value == 100.5f);
    return 0;
}

int main(int argc, const char *argv[])
{
    CanopyContext canopy;
    CanopyResultEnum result;
    RedTest test;
    int i;

    test = RedTest_Begin(argv[0], NULL, NULL);

    canopy = canopy_init_context();
    RedTest_Verify(test, "Canopy init", canopy);

    result = canopy_set_opt(canopy,
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21a",
        CANOPY_SYNC_BLOCKING, true,
        CANOPY_SYNC_TIMEOUT_MS, 10000,
        CANOPY_VAR_SEND_PROTOCOL, CANOPY_PROTOCOL_WS,
        CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_WS
    );
    RedTest_Verify(test, "Configure canopy options", result == CANOPY_SUCCESS);

    result = canopy_var_init(canopy, "inout float32 dimmer_level");
    RedTest_Verify(test, "Configure cloud variable", result == CANOPY_SUCCESS);

    result = canopy_var_on_change(canopy, "dimmer_level", handle_dimmer_level, test);
    RedTest_Verify(test, "Establish on_change callback", result == CANOPY_SUCCESS);

    for (i = 0; i < 100; i++)
    {
        printf("sync...\n");
        result = canopy_sync(canopy, NULL);
    }
    RedTest_Verify(test, "Callback Triggered", callbackTriggered);
    return RedTest_End(test);
}
