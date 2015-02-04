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
#include "log/st_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new STOptions object with all options unset.
STOptions st_options_new_empty()
{
    return canopy_os_calloc(1, sizeof(struct STOptions_t));
}

#define _OPTION_SET_AND_FREE_OLD(options, prop, szVal) \
    do { \
        if ((options)->has_##prop) \
        { \
            canopy_os_free((options)->val_##prop); \
        } \
        (options)->has_##prop = true; \
        (options)->val_##prop = canopy_os_strdup(szVal); \
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
    options = canopy_os_calloc(1, sizeof(struct STOptions_t));
    if (!options)
    {
        return NULL;
    }

    _OPTION_SET_AND_FREE_OLD(options, CANOPY_CLOUD_SERVER, "canopy.link");
    _OPTION_SET(options, CANOPY_HTTP_PORT, 80);
    _OPTION_SET(options, CANOPY_HTTPS_PORT, 443);
    _OPTION_SET(options, CANOPY_SKIP_SSL_CERT_CHECK, false);
    _OPTION_SET(options, CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_WSS);
    _OPTION_SET(options, CANOPY_VAR_RECV_PROTOCOL, CANOPY_PROTOCOL_WSS);

    return options;
}
STGlobalOptions st_global_options_new_default()
{
    STGlobalOptions options;
    options = canopy_os_calloc(1, sizeof(struct STGlobalOptions_t));
    if (!options)
    {
        return NULL;
    }

#if __linux__
    {
        char filename[1024];
        snprintf(filename, 1024, "%s/.canopy/log", getenv("HOME"));
        _OPTION_SET_AND_FREE_OLD(options, CANOPY_LOG_FILE, filename);
    }
#endif
    _OPTION_SET(options, CANOPY_LOG_ENABLED, true);
    _OPTION_SET(options, CANOPY_LOG_LEVEL, 2);
    _OPTION_SET(options, CANOPY_LOG_PAYLOADS, false);

    return options;
}
STVarOptions st_var_options_new_default()
{
    STVarOptions options;
    options = canopy_os_calloc(1, sizeof(struct STVarOptions_t));
    if (!options)
    {
        return NULL;
    }

    _OPTION_SET(options, CANOPY_VAR_DATATYPE, CANOPY_DATATYPE_FLOAT32);
    _OPTION_SET(options, CANOPY_VAR_DIRECTION, CANOPY_DIRECTION_INOUT);

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
    #define _OPTION_LIST_FOREACH(opt, datatype, va_datatype, freefn, fromstring) \
        dest->val_##opt = override->has_##opt ? override->val_##opt : base->val_##opt; \
        dest->has_##opt = base->has_##opt || override->has_##opt;

    _OPTION_LIST
}

CanopyResultEnum st_options_extend_varargs(STOptions base, va_list ap)
{
    // TODO: Call free routines
    // TODO: Call duplicate routine
    //
    // This macro causes _OPTION_LIST to expand to something like:
    //
    //      case CANOPY_CLOUD_SERVER:
    //      {
    //          base->val_CANOPY_CLOUD_SERVER = va_arg(ap, char *);
    //          base->has_CANOPY_CLOUD_SERVER = true;
    //          break;
    //      }
    #undef _OPTION_LIST_FOREACH
    #define _OPTION_LIST_FOREACH(opt, datatype, va_datatype, freefn, fromstring) \
        case opt: \
        { \
            base->val_##opt = (datatype)va_arg(ap, va_datatype); \
            base->has_##opt = true; \
            break; \
        }

    CanopyOptEnum param;
    while ((param = (CanopyOptEnum)va_arg(ap, int)) != CANOPY_OPT_LIST_END)
    {
        switch (param)
        {
            _OPTION_LIST
            default:
            {
                return CANOPY_ERROR_INVALID_OPT;
            }
        }
    }
    return CANOPY_SUCCESS;
}
CanopyResultEnum st_global_options_extend_varargs(STGlobalOptions base, va_list ap)
{
    CanopyGlobalOptEnum param;
    while ((param = (CanopyGlobalOptEnum)va_arg(ap, int)) != CANOPY_GLOBAL_OPT_LIST_END)
    {
        switch (param)
        {
            _GLOBAL_OPTION_LIST
            default:
            {
                return CANOPY_ERROR_INVALID_OPT;
            }
        }
    }
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_var_options_extend_varargs(STVarOptions base, va_list ap)
{
    CanopyVarConfigEnum param;
    while ((param = (CanopyVarConfigEnum)va_arg(ap, int)) != CANOPY_VAR_OPT_LIST_END)
    {
        switch (param)
        {
            _VAR_OPTION_LIST
            default:
            {
                return CANOPY_ERROR_INVALID_OPT;
            }
        }
    }
    return CANOPY_SUCCESS;
}

CanopyResultEnum st_options_new_extend_varargs_impl(STOptions *newOptions, STOptions base, va_list ap)
{
    *newOptions = st_options_dup(base);
    if (!*newOptions)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    // TODO: broken?
    st_options_extend_varargs(*newOptions, ap);
    return CANOPY_SUCCESS;
}

// Free STOption object.
void st_options_free(STOptions options)
{
    canopy_os_free(options);
}

STOptions st_options_dup(STOptions options)
{
    STOptions out;
    out = st_options_new_empty();
    if (!out)
    {
        return NULL;
    }
    memcpy(out, options, sizeof(struct STOptions_t));
    return out;
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
    #define _OPTION_LIST_FOREACH(opt, datatype, va_datatype, freefn, fromstring) \
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
    #define _OPTION_LIST_FOREACH(opt, datatype, va_datatype, freefn, fromstring) \
        case opt: \
            return options->has_##opt;

    switch (option)
    {
        _OPTION_LIST
        default:
            st_log_error("Invalid option to st_option_is_set: %d\n", option);
            return false;
    }
}
