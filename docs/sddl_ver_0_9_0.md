SDDL - v0.90
==============================================================================

Introduction
-----------------------------------------------------------------------------=

The Smart Device Description Language (SDDL) is a JSON-based document format
used by Canopy.  SDDL lets you configure "Cloud Variables" and group them into
hierarchical structures.  Think of SDDL as a "type system" for IoT that
declares the sensors, controls, and configuration variables for a smart
product.

SDDL also allows you to define abstract "interfaces" and implement others'
interfaces.  For example, a device with an embedded GPS module can (and should)
implement the "canopy.common.gps" interface.  This allows 3rd-party
applications to access the device's GPS sensor in a standardized way.

### Example

The following example shows an SDDL document for a multi-sensor that has two
temperature sensors, a humidity sensore, a GPS sensor, and can be remotely
rebooted:

    {
        in bool reboot : { },

        out namedtuple gps : {
            float32 latitude : {
                min-value : -90,
                max-value : 90,
                unit : "degrees"
            },
            float32 longitude : {
                min-value : -180,
                max-value : 180,
                unit : "degrees"
            }
        },

        out float32 temperature[2] : {
            min-value : -50,
            max-value : 200,
            units : "degrees_c"
        },

        out float32 humidity : {
            min-value : 0.0,
            max-value : 1.0,
            numeric-display-hint : "percentage"
        }
    }

SDDL Document
-----------------------------------------------------------------------------=

SDDL documents are JSON-encoded.  Each SDDL document contains a single
top-level JSON object, beginning with `{` and ending with `}`.  This top-level
object may contain 0 or more keys.

For example, here is the simplest allowable SDDL document:

    {
    }

A typical SDDL document will contain several keys which declare Cloud
Variables.

Cloud Variable Datatypes
-----------------------------------------------------------------------------=
SDDL supports a number of datatypes.  These datatypes can be broadly classified
as "basic" and "composite".

The basic datatypes are:

    bool
    datetime
    float32
    float64
    int8
    int16
    int32
    string
    uint8
    uint16
    uint32

The composite datatypes are: 

    struct
    fixed-length array
    variable-length array
    associative array

Declaring a Basic Cloud Variable
-----------------------------------------------------------------------------=

JSON keys are used to declare Cloud Variables.  For a basic Cloud Variable, the
declaration has the following format:

    [<QUALIFIERS>, ...] <DATATYPE> <NAME> : {...}

Examples:

    float32 brightness : {}

    outbound datetime last_boot : {}

    required outbound float64 accel_z : {}

    inbound string owners_firstname : {}

## Datatype Qualifiers

### Directional Qualifier

The directional qualifier specifies which direction data flows, and thereby who
can modify the Cloud Variable's value:

    outbound

        Only the device can change this Cloud Variable's value.

    inbound

        Only the cloud can change this Cloud Variable's value.

    bidrectional (default)

        The device and cloud can both change this Cloud Variable's value.

### Optionality Qualifier

This qualifier specifies whether payloads that contain the Cloud Variable's
parent are required to also contain the Cloud Variable itself.

    optional (default)

        This Cloud Variable is optional, meaning it does not need to be present
        in payloads.

    required

        This Cloud Variable must be present in any payloads that contain the
        Cloud Variable's parent.


### Basic Datatype

Following any and all qualifiers comes the Cloud Variable's datatype.  This
must be one of the following:

    bool
    datetime
    float32
    float64
    int8
    int16
    int32
    string
    uint8
    uint16
    uint32

### Variable Name

After the datatype comes the Cloud Variable's name.  The name must contain only
the characters A-Z, a-z, 0-9, and _ (underscore).  Case is ignored but by
convention all lowercase is used.


Structs
------------------------------------------------------------------------------

A struct is a grouping of Cloud Variables.  Structs are, themselves, Cloud
Variables, and may be nested arbitrarily.

Fixed-length Array
------------------------------------------------------------------------------
Fixed-length arrays provide repetition:

    required outbound float32 cpu_utilization[4] : {
        min-value: 0.0,
        max-value: 1.0
    }

Arrays of structs are allowed, enabling you to construct multi-dimensional
arrays.

    struct row[10] {
        struct col[5] {
            float32 brightness : {}
        }
    }

Variable-length arrays
------------------------------------------------------------------------------
Variable-length arrays serve as "lists":

    struct call_log[] : {
        required string phone_numer : {}
        string recipient_name : {},
        float32 duration : {}
    }

Maps
------------------------------------------------------------------------------
Maps provide associative arrays.

    inbound map[string:struct] favorite_stocks : {
        datetime last_trade : {},
        float64 last_value : {}
    }
