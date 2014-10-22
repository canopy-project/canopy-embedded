#include <canopy.h>

int main(void)
{
    CanopyResultEnum result;
    float temperature;

    result = canopy_global_config(
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "c31a8ced-b9f1-4b0c-afe9-1afed3b0c21f"
    );
    if (result != CANOPY_SUCCESS)
    {
        fprintf(stderr, "Error configuring global ctx\n");
        return -1;
    }

    result = canopy_var_config(
        CANOPY_VAR_NAME, "temperature",
        CANOPY_VAR_DATATYPE, CANOPY_FLOAT64
    );
    if (result != CANOPY_SUCCESS)
    {
        fprintf(stderr, "Error configuring cloud variable \"temperature\"\n");
        return -1;
    }

    result = canopy_sync();
    if (result != CANOPY_SUCCESS)
    {
        fprintf(stderr, "Error syncing with the cloud\n");
        return -1;
    }

    result = canopy_var_read(
        CANOPY_VAR_NAME, "temperature",
        CANOPY_STORE_VALUE_FLOAT32, &temperature);
    if (result == CANOPY_SUCCESS)
    {
        printf("Value of temperature: %f\n", temperature);
    }
    
    return (result == CANOPY_SUCCESS) ? 0 : -1;
}
