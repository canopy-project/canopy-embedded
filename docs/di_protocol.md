Canopy Device Interface Protocol
==============================================================================

Introduction
-----------------------------------------------------------------------------=

Canopy Device Interface Protocol is the protocol used for communications
between the device and the cloud.  Payloads can be sent over HTTP or
WebSockets.  The payloads are similar, but slightly different depending on
whether HTTP or WebSockets are used.


Sync
-----------------------------------------------------------------------------=

When `canopy_sync` is called, the following payload is sent:

    {
        "device_id" : "a943...",
        "var_config" : {
            // SDDL 
        },
        "vars" : {
            "temperature" : 38.0f;
            "gps" : {
                "latitude" : 38.0f;
                "longitude" : 38.0f;
            }
        }
    }

    Only variables that are "dirty" and ("outbound" or "bidirectional") are
    included in the payload.

    The Cloud Server sends the following:

    {
        "var_config" : {
            // SDDL
        },
        "vars" : {
            "temparature" : {
                "t" : DATETIME,
                "v" : VALUE
            }
        }
    }

WS Handshake:

    WS SEND 
    {
        "device_id" : "a943...",
    }

TODO: Add timing element?
