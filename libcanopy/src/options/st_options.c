// Copyright 2014 SimpleThings, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Configuration state manager for Canopy contexts and routines.

#include "options/st_options.h"
#include "red_log.h"
#include "red_string.h"
#include <stdlib.h>

// _OPTION_LIST allows you to write macros that easily perform an action on all
// configuration options.  Whenever you type _OPTION_LIST its expansion depends
// on the currently-defined value of _OPTION_LIST_FOREACH.  So by redefining
// _OPTION_LIST_FOREACH you can easily generate code for the whole list.
//
//                       ENUM VALUE,  DATATYPE,  FREE_ROUTINE
#define _OPTION_LIST \
    _OPTION_LIST_FOREACH(CANOPY_CLOUD_SERVER, char *, free) \
    _OPTION_LIST_FOREACH(CANOPY_CONTROL_PROTOCOL, CanopyProtocolEnum, _noop) \
    _OPTION_LIST_FOREACH(CANOPY_DEVICE_UUID, char *, free) \
    _OPTION_LIST_FOREACH(CANOPY_NOTIFY_MSG, char *, free) \
    _OPTION_LIST_FOREACH(CANOPY_NOTIFY_PROTOCOL, CanopyProtocolEnum, _noop) \
    _OPTION_LIST_FOREACH(CANOPY_NOTIFY_TYPE, CanopyNotifyTypeEnum, _noop) \
    _OPTION_LIST_FOREACH(CANOPY_PROPERTY_NAME, char *, free) \
    _OPTION_LIST_FOREACH(CANOPY_REPORT_PROTOCOL, CanopyProtocolEnum, _noop)

#define _OPTION_LIST_FOREACH(option, datatype, freefn) 

// Generate STOptions_t structure.
// The macro causes _OPTION_LIST to eexpand to something like:
//
//      bool has_CANOPY_CLOUD_SERVER;
//      char * val_CANOPY_CLOUD_SERVER;
//
//      bool has_CANOPY_CONTROL_PROTOCOL;
//      CanopyProtocolEnum val_CANOPY_CONTROL_PROTOCOL;
//
//      ...
#undef _OPTION_LIST_FOREACH
#define _OPTION_LIST_FOREACH(option, datatype, freefn) \
        bool has_##option; \
        datatype val_##option;
 
typedef struct STOptions_t
{
    _OPTION_LIST
} STOptions_t;

// Create a new STOptions object with all options unset.
STOptions st_options_new_empty()
{
    return calloc(1, sizeof(struct STOptions_t));
}

#define _OPTION_SET_AND_FREE_OLD(options, prop, szVal) \
    do { \
        if ((options)->has_##prop) \
        { \
            free((options)->val_##prop); \
            (options)->has_##prop = true; \
            (options)->val_##prop = RedString_strdup(szVal); \
        } \
    } while (0)

#define _OPTION_SET(options, prop, szVal) \
    do { \
        (options)->has_##prop = true; \
        (options)->val_##prop = szVal; \
    } while (0)

// Create a new STOptions object with default values for all options.
STOptions st_options_new_default()
{
    STOptions options;
    options = calloc(1, sizeof(struct STOptions_t));
    if (!options)
    {
        return NULL;
    }

    _OPTION_SET_AND_FREE_OLD(options, CANOPY_CLOUD_SERVER, "canopy.link");
    _OPTION_SET(options, CANOPY_CONTROL_PROTOCOL, CANOPY_PROTOCOL_WS);
    _OPTION_SET(options, CANOPY_NOTIFY_PROTOCOL, CANOPY_PROTOCOL_HTTP);
    _OPTION_SET(options, CANOPY_NOTIFY_TYPE, CANOPY_NOTIFY_MED_PRIORITY);
    _OPTION_SET(options, CANOPY_REPORT_PROTOCOL, CANOPY_PROTOCOL_HTTP);

    return options;
}

// Merge two STOptions objects, by starting with <base> and overriding all
// options that are set in <override>.  Store the result in <dest>.  It is ok
// for <dest> to be the same as <base> or <override>.
void st_options_extend(STOptions dest, STOptions base, STOptions override)
{
    // TODO: Call free routines

    // This macro causes _OPTION_LIST to expand to something like:
    //
    //      dest->val_CANOPY_CLOUD_SERVER = 
    //          override->has_CANOPY_CLOUD_SERVER ?
    //              override->val_CANOPY_CLOUD_SERVER,
    //              base->val_CANOPY_CLOUD_SERVER;
    //      dest->has_CANOPY_CLOUD_SERVER = 
    //          base->has_CANOPY_CLOUD_SERVER || override->has_CANOPY_CLOUD_SERVER;
    #undef _OPTION_LIST_FOREACH
    #define _OPTION_LIST_FOREACH(opt, datatype, freefn) \
        dest->val_##opt = override->has_##opt ? override->val_##opt : base->val_##opt; \
        dest->has_##opt = base->has_##opt || override->has_##opt;

    _OPTION_LIST
}

// Free STOption object.
void st_options_free(STOptions options)
{
    free(options);
}

char *st_option_enum_to_string(CanopyOptEnum option)
{
    // This macro causes _OPTION_LIST to expand to something like:
    //
    //      case CANOPY_CLOUD_SERVER:
    //          return "CANOPY_CLOUD_SERVER";
    //      case CANOPY_CONTROL_PROTOCOL":
    //          return "CANOPY_CONTROL_PROTOCOL";
    //      ...
    #undef _OPTION_LIST_FOREACH
    #define _OPTION_LIST_FOREACH(opt, datatype, freefn) \
        case opt: \
            return #opt;

    switch (option)
    {
        _OPTION_LIST
        default:
            return "<invalid CanopyOptEnum value>";
    }
}

bool st_option_is_set(STOptions options, CanopyOptEnum option)
{
    // This macro causes _OPTION_LIST to expand to something like when:
    //
    //      case CANOPY_CLOUD_SERVER:
    //          return options->has_CANOPY_CLOUD_SERVER;
    //      case CANOPY_CONTROL_PROTOCOL;
    //          return val_CANOPY_CONTROL_PROTOCOL;
    //      ...
    #undef _OPTION_LIST_FOREACH
    #define _OPTION_LIST_FOREACH(opt, datatype, freefn) \
        case opt: \
            return options->has_##opt;

    switch (option)
    {
        _OPTION_LIST
        default:
            RedLog_Error("Invalid option to st_option_is_set: %d\n", option);
            return false;
    }
}
