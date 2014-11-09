Canopy Embedded Configuration (C/C++)
==============================================================================

The libcanopy library is highly configurable.  There are many options that
control the behaviour of libcanopy routines.  The options fall into a few
categories:

*Types of Options*:

 - *Global Options* - Affect all libcanopy routines.
 - *Context Options* - Affect a particular `CanopyContext`.
 - *Cloud Variable Configuration* - Affects a particular Cloud Variable only.
 - *Method Options* - Affects a particular routine or method only.

The libcanopy library supports multiple methods for configuring options:

 - Runtime configuration routines
 - Environment variables
 - Configuration file
 - Hard-coded defaults

Global Options
------------------------------------------------------------------------------

Global options control the overall behavior of libcanopy.  The following
options are supported:

    CANOPY_LOG_ENABLED

        (boolean, default: true)
        Enable/disable logging within libcanopy.

    CANOPY_LOG_FILE

        (string, default: "~/.canopy/log")
        Filename of log file to write to.

    CANOPY_LOG_LEVEL
        
        (integer, default: 2)
        0 = Trace, Debug, Info, Warn, Error & Fatal messages
        1 = Debug, Info, Warn, Error & Fatal messages
        2 = Info, Warn, Error & Fatal messages
        3 = Warn, Error & Fatal messages
        4 = Error & Fatal messages
        5 = Fatal messages only

    CANOPY_LOG_PAYLOADS

        (boolean, default: false)
        If true, communication payloads will be included in the log as DEBUG
        messages.

## Setting Global Options at runtime:

You can set Global Options at runtime using `canopy_set_global_opt`.  Just
provide one or more (KEY, VALUE) pairs like so:

    canopy_set_global_opt(
            CANOPY_LOG_ENABLED, true,
            CANOPY_LOG_FILE, "/var/log/mylogfile.log");

## Setting Global Options using Environment Variables:

Global Options can be configured using environment variables.  For example:

    CANOPY_LOG_PAYLOADS=true ./myprogram

Context Options
------------------------------------------------------------------------------

The libcanopy library lets you create one or more Contexts which contain
internal state used by the library.  Each Context has several options which
govern its behaviour:
