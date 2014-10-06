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

typedef struct STOptions_t * STOptions;

// Create a new STOptions object with all options unset.
STOptions st_options_new_empty();

// Create a new STOptions object with default values for all options.
STOptions st_options_new_default();

// Merge two STOptions objects, by starting with <base> and overriding all
// options that are set in <override>.  Store the result in <dest>.  It is ok
// for <dest> to be the same as <base> or <override>.
void st_options_extend(STOptions dest, STOptions base, STOptions override);

// Free STOption object.
void st_options_free(STOptions options);

// Does STOptions object have a particular option set?
bool st_option_is_set(STOptions options, CanopyOptEnum option);

#endif // ST_OPTIONS_INCLUDED
