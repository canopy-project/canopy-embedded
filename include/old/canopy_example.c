#include "canopy_new.h"

int main(void)
{
    CanopyContext ctx;
    CanopyResultEnum result;

    ctx = canopy_init_context();
    if (!ctx)
    {
        canopy_print_error(ctx);
        return -1;
    }

    result = canopy_opts(ctx,
        CANOPY_CLOUD_SERVER, "canopy.link",
        CANOPY_UUID, "93084928498240842984209");
    if (result)
    {
        canopy_print_error(ctx);
        return -1;
    }

    canopy_var_set(ctx, "temperature", CANOPY_FLOAT32(84.f));
    canopy_var_set(ctx, 
        "gps", CANOPY_STRUCT(
            "latitude", CANOPY_FLOAT32(84.0f)
            "longitude", CANOPY_INT8(8.0f)
        )
    );

    canopy_var_set(ctx, "accelerometers", 
        CANOPY_ARRAY(
            CANOPY_STRUCT(
                "x", CANOPY_FLOAT32(9),
                "y", CANOPY_FLOAT32(10),
                "z", CANOPY_FLOAT32(30)
            ),
            CANOPY_STRUCT(
                "x", CANOPY_FLOAT32(3),
                "y", CANOPY_FLOAT32(8),
                "z", CANOPY_FLOAT32(10)
            )
        )
    );

    canopy_var_get(ctx, "temperature", CANOPY_FLOAT32_PTR(&temperature));
    canopy_var_get(ctx, "gps", CANOPY_STRUCT(
        "latitude", CANOPY_FLOAT32_PTR(&latitude),
        "longitude", CANOPY_FLOAT32_PTR(&longitude)
    );

    canopy_var_on_change(ctx, "gps", handle_gps);

    canopy_var_set(ctx, "accelerometers[3].y", CANOPY_FLOAT32(43.0f));

    canopy_sync(ctx);
}
