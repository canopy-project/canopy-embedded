IoT Cloud Variables in C
===============================================================================

Overview
-------------------------------------------------------------------------------

### Purpose

Many IoT systems require device state to be stored in the cloud.  This "state"
may include sensor readings, device configuration, and control settings.

Cloud Variables make it simple for firmware developers to store and access data
that is stored in the cloud.  Cloud Variables can be created, read, updated,
and deleted by making C function calls from within device firmware.
Internally, WebSockets and/or HTTP/REST are used to communicate with a server
running the Canopy Cloud Service.

### Example of IoT Cloud Variables in C

This program demonstrates some of the ways in which Cloud Variables can be
used, for a hypothetical "smart toaster oven".

```c
#include <canopy.h>

// Callback for handling changes to the "darkness" Cloud Variable.
int handle_darkness(CanopyContext ctx, const char *varName, float value)
{
    // Your custom code for handling a change to the toast darkness level goes
    // here.
    // Perhaps you set GPIO pins here.
    // ...
    return 0;
}

// Callback for handling changes to the "bagel_mode" Cloud Variable.
int handle_darkness(CanopyContext ctx, const char *varName, float value)
{
    // Your device-specific code goes here.
    // ...
    return 0;
}

int main(void)
{
    // Configure libcanopy's global context.  The options set here are used by
    // all subsequent routines.  Options are provided as "key, value" pairs of
    // parameters.
    canopy_global_config(
        // Communicate with a test server on port 8080.
        // By default, the server "canopy.link" is used on port 443.
        CANOPY_CLOUD_SERVER, "test4.canopy.link:8080",

        // Use WebSockets for outbound communications.
        // By default, HTTPS is used.
        CANOPY_VAR_OUTBOUND_PROTOCOL, CANOPY_PROTOCOL_WS,

        // Use a hardcoded UUID to identify this device.
        // If not set, then Canopy checks for a UUID in the environment
        // variable CANOPY_UUID, or in the file "~/.canopy/uuid" or lastly in
        // the file "/etc/canopy/uuid".
        CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e6"
    )

    // Register a callback that gets triggered whenever the value of the
    // "darkness" Cloud Variable changes.
    // This also causes a floating-point Cloud Variable called "darkness" to
    // get created the first time the program runs.
    canopy_var_on_change(
        CANOPY_VAR_NAME, "darkness",
        CANOPY_VAR_CALLBACK_FLOAT32, handle_darkness
    );

    // Register a callback that gets triggered whenever the value of the
    // "bagel_mode" Cloud Variable changes.
    // This also causes a boolean Cloud Variable called "bagel_mode" to get
    // created the first time the program runs.
    canopy_var_on_change(
        CANOPY_VAR_NAME, "bagel_mode",
        CANOPY_VAR_CALLBACK_BOOL, handle_darkness
    );

    // Run forever, taking sensor readings every 10 seconds.
    while (1)
    {
        CanopyResult result;

        // ... your device-specific code for reading the internal temperature
        // here.  For now, we just fake it:
        float internalTemperature = 284.34f;
        
        // Report the internal temperature of the toaster to the cloud.
        // The actual reporting happens when canopy_sync is called.
        canopy_var_set(
            CANOPY_VAR_NAME, "itemp",
            CANOPY_VALUE_FLOAT32, internalTemperature);

        // Synchronize with the cloud.  This is where actual communication with
        // the cloud happens.
        result = canopy_sync(
            // Don't return immediately.
            CANOPY_SYNC_BLOCKING, true

            // Stay inside the canopy_sync routine for at least 10 seconds.
            // This is better than using "sleep", because some work (like
            // servicing the websockets) is performed while waiting.
            CANOPY_SYNC_DURATION_MS, 10000);

        // Log any errors that may have occurred
        if (result != CANOPY_SUCCESS)
        {
            fprintf(stderr, "Error: %s\n", canopy_global_error_string());
        }
    }
}
```

### Important Concepts

Canopy Cloud Variables are very easy to use.  That said, there is a lot going
on behind the scenes that you should know about to have a complete
understanding of how to work with them.

### Device Identity

The Canopy Cloud Service requires every device to have a Type-4 UUID, such as:

    c4aa0aa5-a752-431c-954b-4a761214e650

If your device doesn't have a UUID, the first call to canopy_sync will generate
a UUID for your device and store it on your filesutem (typically in
`"~/.canopy/uuid`).

Alternatively, you can generate the UUID for your device.  You can use the
linux command-line `uuid` tool:

    uuid -v4

To configure `libcanopy` to use it the UUID you generated, there are a few
options:

 - **Environment Variable:** Set the `DEVICE_UUID` environment variable.
 - **UUID config file:** Create a file containing the UUID (and nothing else)
    in the appropriate place on your file system (typically `~/.canopy/uuid` or
    `/etc/canopy/uuid`).
 - **At runtime:** Call `canopy_global_config(CANOPY_DEVICE_UUID, "c4aa...");`
    or a similar routine.

Generall

### Cloud Synchronization

A device may have several Cloud Variables associated with it.  A copy (or
"mirror") of each Cloud Variable is stored in your program's memory as
part of the `CanopyContext` object.

Any operation on a Cloud Variable fundamentally requires two steps.
 - **Local Step**
 - **Sync Step**

For example, modifying a Cloud Variable involves:

1. **Local Step**: Modifying the local copy.
2. **Sync Step**: Updating the cloud's copy.

Reading a Cloud Variable also involves two steps:

1. **Sync Step**: Fetching latest data from the cloud into the local copy.
2. **Local Step** Reading the local copy.

The **Sync Step** is performed by calling `canopy_sync()`.  Don't forget to
call it!  If you omit the `canopy_sync()` call, your program won't actually do
anything useful.

Most other
operations occur immediately because 

Examples
-------------------------------------------------------------------------------
### Creation
A Canopy Cloud Variable is created by simply setting its value for the first
time:
```c
#include <canopy.h>

int main(void)
{
    // YOUR SENSOR-READING CODE GOES HERE
    // We'll just fake it:
    float temperature = 97.8f;

    // This creates a Cloud Variable called "temperature" if it doesn't already
    // exist for this device:
    canopy_var_set(
        CANOPY_VAR_NAME, "temperature", 
        CANOPY_VALUE_FLOAT32, temperature);

    return 0;
}
```

### Existence Checking
You can check if a Canopy Cloud Variable exists with:

``` c
#include <canopy.h>

int main(void)
{
    if (canopy_var_exists(CANOPY_VAR_NAME, "temperature"))
}
