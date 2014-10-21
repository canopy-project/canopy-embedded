#include <canopy.h>

int main(void)
{
    CanopyResultEnum result = canopy_sync(
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f"
    );
    return (result == CANOPY_SUCCESS) ? 0 : -1;
}
