#include <canopy.h>

int main(void)
{
    CanopyResultEnum result;
    result = canopy_global_config(
            CANOPY_CLOUD_SERVER, "dev02.canopy.link",
            CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f"
    );
    if (result != CANOPY_SUCCESS)
    {
        fprintf(stderr, "Error configuring canopy\n");
        return -1;
    }

    result = canopy_sync();
    return (result == CANOPY_SUCCESS) ? 0 : -1;
}
