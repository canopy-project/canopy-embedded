This is outdated.  See canopy.h for latest documentation.

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

Using IoT Cloud Variables can be as simple as adding one line of code to your
program:

```c
#include <canopy.h>

int main(void)
{  
    // Device-specific sensor reading code goes here
    float humidity = 0.68f;

    // Store the device's current humidity measurement in the cloud
    canopy_var_set(
        CANOPY_VAR_NAME, "humidity",
        CANOPY_VALUE_FLOAT32, humidity,
        CANOPY_AUTO_SYNC, true
    );

    return 0;
}
```

When you run this program, a Cloud Variable called `humidity` is initialized
and assigned the value `0.68f`.  This value is exposed to the web by going to:

    http://canopy.link/api/device/&lt;ID&gt;/vars/humidity

Here is another example.  This program demonstrates some of the ways in which
Cloud Variables might be used in the firmware for a hypothetical "smart
toaster":

```c
#include <canopy.h>
#include <stdio.h>

// Callback for handling changes to the "darkness" Cloud Variable.
static int handle_darkness(CanopyContext ctx, const char *varName, float value, void *extra)
{
    // Your custom code for handling a change to the toast darkness level goes
    // here.
    // Perhaps you set GPIO pins here.
    // ...
    return 0;
}

// Callback for handling changes to the "bagel_mode" Cloud Variable.
static int handle_bagel_mode(CanopyContext ctx, const char *varName, float value, void *extra)
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
    );

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
        CANOPY_VAR_CALLBACK_BOOL, handle_bagel_mode
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

Alternatively, you can generate the UUID for your device.  One option is to use
the linux command-line `uuid` tool:

    uuid -v4

There are a few ways to configure `libcanopy` to use the UUID you generated:

 - **Environment Variable:** Set the `DEVICE_UUID` environment variable.
 - **UUID config file:** Create a file containing the UUID (and nothing else)
    in the appropriate place on your file system (typically `~/.canopy/uuid` or
    `/etc/canopy/uuid`).
 - **At runtime:** Call `canopy_global_config(CANOPY_DEVICE_UUID, "c4aa...");`
    or a similar routine.

### Canopy Context

Most of the internal state used by `libcanopy` is contained in a `CanopyContext
object`. This internal state includes:

 - A local mirror of each Cloud Variable for your device.
 - Configuration settings that guide the behaviour of `canopy_` routines.
 - State for maintaining connections to the Canopy Cloud Service.

Every `libcanopy` routine operates on a `CanopyContext`, which may be either:

 - The **Global Context**: an automatically initialized Context.
 - A **User-created Context**.

Some routines only operate on the global context:

```c
    // Configure the Global Context
    canopy_global_config(CANOPY_AUTO_SYNC, true);
```

Some routines only operate on user-created Contexts:

```c
    // Create a user-created context:
    CanopyContext ctx;
    ctx = canopy_create_ctx();
    assert(ctx);

    // Configure the user-created context:
    canopy_ctx_config(ctx, CANOPY_AUTO_SYNC, true);
```

Some routines can operate on either the Global Context or a user-created
Context:

```c
    // Update a Cloud Variable using the global context:
    canopy_set_var(
        CANOPY_VAR_NAME, "battery_level", 
        CANOPY_VALUE_FLOAT32, 0.1f
    );

    // Update a Cloud Variable using a user-created context.
    canopy_set_var(
        CANOPY_CTX, ctx,
        CANOPY_VAR_NAME, "cpu_level", 
        CANOPY_VALUE_FLOAT32, 0.15f
    );

```

In most cases, it is simplest to only use the global context.  For some
applications, such as device simulations & testing of `libcanopy`, it is useful
to be able to create multiple contexts within a single program.

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

With `CANOPY_AUTO_SYNC`, you can cause the Sync Step to automatically occur
alongside every Local Step.  While convenient, this may result in an excessive
amount of communication with the server, and is not recommended.


### Asynchronous Routines & Promises

Operations that perform a **Sync Step**, such as `canopy_sync()`, block your
program's execution until communication with the server has completed.  If
you would rather have these routines return immediately, you can use a `Canopy
Promise` for finer-grained control over synchronization:

```c
    CanopyPromise promise;

    // When the CANOPY_PROMISE option is provided, the `canopy_sync` routine
    // will return immediately after kicking off operations in another thread.
    // A newly-allocated CanopyPromise object gets created that can be used
    // later for synchronization.
    canopy_sync(CANOPY_PROMISE, &promise);

    // .. do stuff

    // Now we can wait for the sync to complete:
    canopy_promise_wait(promise, CANOPY_TIMEOUT, 10.0f);
    if (canopy_promise_result(promise) == CANOPY_SUCCESS)
    {
        printf("Syncronized with server!\n");
    }
    else
    {
        printf("Error synchronizing with server!\n");
        canopy_print_error();
    }

    // Don't forget to free the Promise object when done using it.
    canopy_destroy_promise(promise);
```


Examples
-------------------------------------------------------------------------------
### Creation
A Cloud Variable is created by simply referencing its name for the first time.
```c
#include <canopy.h>

int main(void)
{
    bool on_off;
    // YOUR SENSOR-READING CODE GOES HERE
    // We'll just fake it:
    float temperature = 97.8f;

    // This creates a Cloud Variable called "temperature" if it doesn't already
    // exist for this device:
    canopy_var_set(
        CANOPY_VAR_NAME, "temperature", 
        CANOPY_VALUE_FLOAT32, temperature);

    canopy_sync();

    // Reading a Cloud Variable for the first time also creates it.
    canopy_var_get(
        CANOPY_VAR_NAME, "on_off",
        CANOPY_VAR_ENABLE_CLOUD_CONTROL, true,
        CANOPY_GET_VALUE_BOOL, &on_off);
    );

    canopy_sync();

    return 0;
}
```

### Configuration Options

Cloud Variables have several properties that can be configured.  In this
example, we change a Cloud Variable's datatype to FLOAT64, change the units,
disable historical data tracking, and modify the application-side
permissions

```c
    canopy_var_config(
        CANOPY_VAR_NAME, "temperature",
        CANOPY_VAR_DATATYPE, CANOPY_FLOAT64,
        CANOPY_VAR_UNITS, "degrees_c",
        CANOPY_VAR_APP_PERMISSIONS, CANOPY_READ_ONLY
    );
    canopy_sync();
```

### Reading
You can read the current value of a CanopyCloud variable by using:

```c
    CanopyResult result;

    canopy_sync();

    result = canopy_var_read(
        CANOPY_VAR_NAME, "temperature",
        CANOPY_GET_VALUE_FLOAT32, &temperature);
    if (result == CANOPY_SUCCESS)
    {
        // Cloud variable value has been stored into &temperature.
    }
    else if (result == CANOPY_ERROR_VAR_DOES_NOT_EXIST)
    {
        // This synchronization will create the cloud variable.
        canopy_sync();
    }
```

Note that `canopy_sync` is called twice.  The first time ensures that the
latest value of "temperature" is fetched, as long as the "temperature" Cloud
Variable already exists.  The second call creates the Cloud Variable if this is
the first time it is being referenced.

The same effect can be achieved with the following code:

```c
    CanopyResult result;

    canopy_var_config(
        CANOPY_VAR_NAME, "temperature",
        CANOPY_VAR_DATATYPE, CANOPY_FLOAT32);

    canopy_sync();

    result = canopy_var_read(
        CANOPY_VAR_NAME, "temperature",
        CANOPY_GET_VALUE_FLOAT32, &temperature);
    if (result == CANOPY_SUCCESS)
    {
        // Cloud variable value has been stored into &temperature.
    }
```

In this example, the `canopy_var_config` ensures that the "temperature" Cloud
Variable gets created if necessary.

### OnChange Callback
You can register a callback that will trigger during `canopy_sync` if the value of a Cloud
Variable changes.

```c

static int handle_dimmer_level(CanopyContext ctx, const char *varName, float value)
{
    printf("Dimmer set to: %f\n", value);
    return 0;
}

int main()
{
    canopy_var_on_change(
        CANOPY_VAR_NAME, "dimmer_level",
        CANOPY_VAR_CALLBACK_FLOAT32, handle_dimmer_level
    );
    while (1)
    {
        canopy_sync();
    }

    return 0;
}
```


### Existence Checking
You can check if a Canopy Cloud Variable exists with:

``` c
    canopy_sync();
    if (canopy_var_exists(CANOPY_VAR_NAME, "temperature"))
    {
        ...
    }
```


API Reference
-------------------------------------------------------------------------------

### canopy_var_set

The `canopy_var_set` function:

 - Creates a local Cloud Variable using the provided name, if none already
   exists.
 - Sets the Cloud Variable's local value.
 - Performs cloud synchronization if CANOPY_AUTO_SYNC is true.

The next time cloud synchronization occurs:

 - Creates a remote Cloud Variable using the provided name, if none already
 exists.
 - Sends the most recent local value to the Cloud.
