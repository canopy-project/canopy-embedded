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
running the Cloud Service.

### Device Identity

The Canopy Cloud Service requires every device to have a Type-4 UUID, such as:

    c4aa0aa5-a752-431c-954b-4a761214e650

You must generate the UUID for your device.  You can use the linux command-line
`uuid` tool:

    uuid -v4

After generating a UUID, you need to configure `libcanopy` to use it.  There
are a few options:

 - **Environment Variable:** Set the `DEVICE_UUID` environment variable.
 - **UUID config file:** Create a file ~/.canopy/uuid or /etc/canopy/uuid that
   contains the UUID (and nothing else).
 - **Runtime:** Call `canopy_global_config(CANOPY_DEVICE_UUID, "c4aa...");` or
   a similar routine.

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
