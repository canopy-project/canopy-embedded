Canopy Cloud Variables
-------------------------------------------------------------------------------

Canopy Cloud Variables enable synchronization of data between firmware, the
cloud, and apps, as well as historical tracking of state changes.

A Canopy Cloud Variable can be created in your firmware or your application by
simply naming it for the first time.

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
    - Tries to determines the device's UUID.  The CANOPY_UUID environment
    variable will be used if set.   Otherwise, the contents of ~/.canopy/uuid
    will be used.  Otherwise, if the file ~/.canopy/uuid doesn't exist, then it
    will be created and a UUID will be generated and assigned.
    - An HTTP Post to http://canopy.link/device/<ID>/var/temperature is
    attempted.


