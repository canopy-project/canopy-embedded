Canopy Cloud Variables
===============================================================================

Canopy Cloud Variables enable synchronization of data between firmware, the
cloud, and apps, while tracking historical state changes.  Canopy Cloud
Variables reside in the cloud and are mirrored in firmware and IoT apps.
They can be used for:

 - Monitoring of sensor data
 - Remote configuration
 - Remote control


Creating A Canopy Cloud Variable
-------------------------------------------------------------------------------
A Canopy Cloud Variable can be created within firmware or your IoT application
by simply setting its value for the first time.

### Creating A Canopy Cloud Variable in C Firmware

This example demonstrates how to create a Canopy Cloud Variable from within
firmware written in C:

```c
#include <canopy.h>

int main(void)
{
    float temperature;

    // YOUR SENSOR-READING CODE GOES HERE
    // We'll just fake it.
    temperature = 97.8f;

    canopy_var_set(
        CANOPY_VAR_NAME, "temperature", 
        CANOPY_VALUE_FLOAT32, temperature);

    return 0;
}
```

The `canopy_var_set` function is a flexible and convenient routine.  The above
call to `canopy_var_set` does all of the following:

  - Initializes the Canopy library and creates a global Canopy context.
  - Tries to determines the device's UUID.  The `CANOPY_UUID` environment
    variable will be used if set.   Otherwise, the contents of `~/.canopy/uuid`
    will be used.  Otherwise, if the file `~/.canopy/uuid` doesn't exist, then
    it will be created and a UUID will be generated and assigned.
  - An HTTP POST to `http://canopy.link/api/device/<ID>/var/temperature` is
    attempted.
  - If the POST succeeds, a Cloud Variable named "temperature" will be created
    and associated with this device in the cloud, or the Cloud Variable will be
    updated if it already exist.

In the above example, many options are unspecified and so defaults are used.
Here are some of the options you can provide:

| Parameter                    -| Description
|-------------------------------|-----------------------------------------------
| `CANOPY_CLOLUD_SERVER`        | Hostname and port of Canopy Cloud Service
| `CANOPY_VAR_OUTBOUND_PROTOCOL`| Communication protocol to use, i.e. HTTP or Websocket
| `CANOPY_CTX`                  | Canopy context to use for defaults
| `CANOPY_DEVICE_UUID`          | Device UUID
| `CANOPY_VAR_NAME`             | Cloud Variable name
| `CANOPY_VALUE_FLOAT32`        | Assign 32-bit floating point value to variable
| `CANOPY_VALUE_FLOAT64`        | Assign 64-bit floating point value to variable
| `CANOPY_VALUE_INT8`           | Assign 8-bit integer value to variable
| `CANOPY_VALUE_UINT8`          | Assign 8-bit unsigned integer value to variable
| `CANOPY_VALUE_INT16`          | Assign 16-bit integer value to variable
| `CANOPY_VALUE_UINT16`         | Assign 16-bit unsigned integer value to variable
| `CANOPY_VALUE_INT32`          | Assign 32-bit integer value to variable
| `CANOPY_VALUE_UINT32`         | Assign 32-bit unsigned integer value to variable


### Creating A Canopy Cloud Variable from Javascript IoT Application

This example demonstrates how to create a Canopy Cloud Variable from within
javascript:

* HTML: *
```html
<script src=canopy_client.js></script>
```

* Javascript: *
```javascript
canopy.device(id).var("brightness").setValue(0.5);
```

### Creating A Canopy Cloud Variable using REST

You can also create a Canopy Cloud Variable using Canopy's REST API.

```json
POST /api/device/api

{
    "vars" : {
        "brightness" : 0.5;
    }
}
```
