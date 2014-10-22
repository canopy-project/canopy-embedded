#include <canopy.h>

static int handle_dimmer_level(CanopyContext ctx, const char *varName, float value)
{
    // TODO: need info about event?
    // Timestamp of change?
    // Origin of change?
    return 0;
}

int main(void)
{
    int i;
    CanopyResultEnum result;
    result = canopy_global_config(
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f"
    );
    if (result != CANOPY_SUCCESS)
    {
        fprintf(stderr, "Failed to configure global context\n");
        return result;
    }

    result = canopy_var_on_change(
        CANOPY_VAR_NAME, "dimmer_level",
        CANOPY_VAR_CALLBACK_FLOAT32, handle_dimmer_level
    );
    if (result != CANOPY_SUCCESS)
    {
        fprintf(stderr, "Failed to setup control callback\n");
        return -1;
    }

    for (i = 0; i < 10; i++)
    {
        printf("sync...\n");
        result = canopy_sync(CANOPY_SYNC_DURATION_MS, 10000);
        if (result != CANOPY_SUCCESS)
        {
            fprintf(stderr, "Error syncing\n");
        }
    }
    return 0;
}
