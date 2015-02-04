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

#ifndef ST_OPTIONS_INCLUDED
#define ST_OPTIONS_INCLUDED

// Configuration state manager for Canopy contexts and routines.
//
// Most of libcanopy's routines rely on configuration state, such as the
// CANOPY_CLOUD_SERVER and CANOPY_DEVICE_UUID settings.  This configuration
// state is stored in the CanopyCtx object.  Most routines also accept
// configuration options in a parameter list, in which case the provided
// options override the CanopyCtx's settings.  For example:
// 
//      int main(void)
//      {
//          // Configure the global context's CANOPY_CLOUD_SERVER option.
//          canopy_global_set_opt(CANOPY_CLOUD_SERVER, "ccs.canopy.link");
//
//          // This call to canopy_notify() uses the global CANOPY_CLOUD_SERVER
//          // value, which is "ccs.canopy.link".
//          canopy_notify(
//                  CANOPY_NOTIFY_TYPE, CANOPY_NOTIFY_SMS,
//                  CANOPY_NOTIFY_MSG, "Low on green toner");
//              
//          // This call to canopy_notify() uses an overridden value for
//          // CANOPY_CLOUD_SERVER.  The override is only for this one function
//          // call.
//          canopy_notify(
//                  CANOPY_CLOUD_SERVER, "test02.canopy.link",
//                  CANOPY_NOTIFY_TYPE, CANOPY_NOTIFY_SMS,
//                  CANOPY_NOTIFY_MSG, "Low on green toner");
//      }
//
// This module handles the parsing, storing, copying, merging, etc. of these
// configuration options.

#include <stdbool.h>
#include <canopy.h>
#include <canopy_os.h>
#include <stdarg.h>

// _OPTION_LIST allows you to write macros that easily perform an action on all
// configuration options.  Whenever you type _OPTION_LIST its expansion depends
// on the currently-defined value of _OPTION_LIST_FOREACH.  So by redefining
// _OPTION_LIST_FOREACH you can easily generate code for the whole list.
//
//                       ENUM VALUE,  DATATYPE,  VARARG_DATATYPE, FREE_ROUTINE, CONVER_FROM_STRING
#define _OPTION_LIST \
    _OPTION_LIST_FOREACH(CANOPY_CLOUD_SERVER, char *, char *, canopy_os_free, (char *)) \
    _OPTION_LIST_FOREACH(CANOPY_DEVICE_UUID, char *, char *, canopy_os_free, (char *)) \
    _OPTION_LIST_FOREACH(CANOPY_DEVICE_SECRET_KEY, char *, char *, canopy_os_free, (char *)) \
    _OPTION_LIST_FOREACH(CANOPY_HTTP_PORT, int, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_HTTPS_PORT, int, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_SKIP_SSL_CERT_CHECK, bool, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_SYNC_BLOCKING, bool, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_SYNC_TIMEOUT_MS, int, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_VAR_SEND_PROTOCOL, CanopyProtocolEnum, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_VAR_RECV_PROTOCOL, CanopyProtocolEnum, int, _noop, atoi)

#define _GLOBAL_OPTION_LIST \
    _OPTION_LIST_FOREACH(CANOPY_LOG_ENABLED, bool, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_LOG_FILE, char *, char *, canopy_os_free, (char *)) \
    _OPTION_LIST_FOREACH(CANOPY_LOG_LEVEL, int, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_LOG_PAYLOADS, bool, int, _noop, atoi)

#define _VAR_OPTION_LIST \
    _OPTION_LIST_FOREACH(CANOPY_VAR_DATATYPE, CanopyDatatypeEnum, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_VAR_DIRECTION, CanopyDirectionEnum, int, _noop, atoi) \
    _OPTION_LIST_FOREACH(CANOPY_VAR_MIN_VALUE, double, double, _noop, atof) \
    _OPTION_LIST_FOREACH(CANOPY_VAR_MAX_VALUE, double, double, _noop, atof)

#define _OPTION_LIST_FOREACH(option, datatype, va_datatype, freefn, fromstring) 

// Generate STOptions_t structure.
// The macro causes _OPTION_LIST to expand to something like:
//
//      bool has_CANOPY_CLOUD_SERVER;
//      char * val_CANOPY_CLOUD_SERVER;
//
//      bool has_CANOPY_CONTROL_PROTOCOL;
//      CanopyProtocolEnum val_CANOPY_CONTROL_PROTOCOL;
//
//      ...
#undef _OPTION_LIST_FOREACH
#define _OPTION_LIST_FOREACH(option, datatype, va_datatype, freefn, fromstring) \
        bool has_##option; \
        datatype val_##option;
 
typedef struct STOptions_t
{
    _OPTION_LIST
} STOptions_t;
typedef struct STOptions_t * STOptions;

typedef struct STGlobalOptions_t
{
    _GLOBAL_OPTION_LIST
} STGlobalOptions_t;
typedef struct STGlobalOptions_t * STGlobalOptions;

typedef struct STVarOptions_t
{
    _VAR_OPTION_LIST
} STVarOptions_t;
typedef struct STVarOptions_t * STVarOptions;

// Create a new STOptions object with all options unset.
STOptions st_options_new_empty();
STGlobalOptions st_global_options_new_empty();
STVarOptions st_var_options_new_empty();

// Create a new STOptions object with default values for all options.
STOptions st_options_new_default();
STGlobalOptions st_global_options_new_default();
STVarOptions st_var_options_new_default();

// Create a new STOptions object using arguments supplied as VARARGS.
// Takes same arguments as va_start.
#define st_options_new_varargs(ap, start) \
    (va_start(ap, start), st_options_new_varargs_impl(ap))
STOptions st_options_new_varargs_impl(va_list ap);

#define st_global_options_new_varargs(ap, start) \
    (va_start(ap, start), st_global_options_new_varargs_impl(ap))
STGlobalOptions st_global_options_new_varargs_impl(va_list ap);

#define st_var_options_new_varargs(ap, start) \
    (va_start(ap, start), st_var_options_new_varargs_impl(ap))
STVarOptions st_var_options_new_varargs_impl(va_list ap);

// Duplicate STOptions.
STOptions st_options_dup(STOptions options);
STGlobalOptions st_global_options_dup(STGlobalOptions options);

// Merge two STOptions objects, by starting with <base> and overriding all
// options that are set in <override>.  Store the result in <dest>.  It is ok
// for <dest> to be the same as <base> or <override>.
void st_options_extend(STOptions dest, STOptions base, STOptions override);

// Merge-in STOptions from varargs.
CanopyResultEnum st_options_extend_varargs(STOptions base, va_list ap);
CanopyResultEnum st_global_options_extend_varargs(STGlobalOptions base, va_list ap);
CanopyResultEnum st_var_options_extend_varargs(STVarOptions base, va_list ap);

#define st_options_new_extend_varargs(newOptions, options, start, ap) \
    (va_start(ap, start), st_options_new_extend_varargs_impl(newOptions, options, ap))
CanopyResultEnum st_options_new_extend_varargs_impl(STOptions *newOptions, STOptions base, va_list ap);

// Free STOption object.
void st_options_free(STOptions options);

// Does STOptions object have a particular option set?
bool st_option_is_set(STOptions options, CanopyOptEnum option);

// Load options from environment variables
void st_options_load_from_env(STOptions options);
void st_global_options_load_from_env(STGlobalOptions options);

#endif // ST_OPTIONS_INCLUDED
