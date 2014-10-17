Canopy Cloud Variables
-------------------------------------------------------------------------------

Canopy Cloud Variables enable synchronization of data between firmware, the
cloud, and apps, while tracking historical state changes.  Canopy Cloud
Variables reside in the cloud and are mirrored in firmware and IoT apps.
They can be used for:

 - Monitoring of sensor data
 - Remote configuration
 - Remote control

Creating A Canopy Cloud Variable
-------------------------------------------------------------------------------
A Canopy Cloud Variable can be created within firmware or your application by
simply setting its value for the first time.

This example demonstrates how to create a Canopy Cloud Variable from within
firmware written in C:

    #include <canopy.h>

    int main(void)
    {
        float temperature;

        // YOUR SENSOR-READING CODE GOES HERE
        temperature = 97.8f;

        canopy_var_set(
            CANOPY_VAR_NAME, "temperature", 
            CANOPY_VALUE_FLOAT32, temperature);

        return 0;
    }

The `canopy_var_set` function call above does all of the following:

  - Initializes the Canopy library and creates a global Canopy context.
  - Tries to determines the device's UUID.  The `CANOPY_UUID` environment
    variable will be used if set.   Otherwise, the contents of `~/.canopy/uuid`
    will be used.  Otherwise, if the file `~/.canopy/uuid` doesn't exist, then
    it will be created and a UUID will be generated and assigned.
  - An HTTP POST to `http://canopy.link/device/<ID>/var/temperature` is
    attempted.
  - If the POST succeeds, a Cloud Variable named "temperature" will be created
    and associated with this device, if it doesn't already exist, or updated if
    it does exist.

