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

#ifndef ST_CLOUDVAR_INTERNAL_INCLUDED
#define ST_CLOUDVAR_INTERNAL_INCLUDED

#include <sddl.h>
#include <red_hash.h>
#include <canopy.h>
#include <time.h>

// Recursive structure representing options passed to canopy_var_init.
//
// Basic variable initialization:
//
//  canopy_var_init(ctx, "inout float32 humidity",
//      CANOPY_VAR_DESCRIPTION, "Current humidity level"
//  );
//
// Array initialization.  All children share all properties, and struct is not
// recursive (unless the element datatype is recursive).
//
//  canopy_var_init(ctx, "out float32[4] cpu_level",
//      CANOPY_VAR_DESCRIPTION, "Usage level for each CPU"
//  );
//
// Struct initialization.
//
//      canopy_var_init(ctx,
//          "out struct ports",
//          CANOPY_VAR_DESCRIPTION, "Outlet port measurements",
//          CANOPY_VAR_STRUCT_MEMBER("float32 amperage"),
//          CANOPY_VAR_STRUCT_MEMBER("float32 voltage"),
//          CANOPY_VAR_STRUCT_MEMBER("struct foo",
//              CANOPY_VAR_STRUCT_MEMBER("out string bar"),
//              CANOPY_VAR_STRUCT_MEMBER("out string baz")
//          )
//      )
//
//      CANOPY_VAR_STRUCT_MEMBER expands to:
//          CANOPY_VAR_CHILD, CANOPY_VAR_CHILD_INIT(...)
// 
typedef struct
{
    // Declaration string, such as "struct[4] ports" or "inout float32 gps"
    char *decl_string;

    // Datatype, as parsed from declstring
    SDDLDatatypeEnum datatype;

    // Direction, as parsed from declstring
    SDDLDirectionEnum direction;

    // Variable name, as parsed from declstring
    char *name;

    // (Array only) Number of array elements
    unsigned array_num_items;
    
    // (Array only) Datatype of array elements
    unsigned array_datatype;

    // Description provided with CANOPY_VAR_DESCRIPTION
    char *description;
} STCloudVarInitOptions_t;

struct STCloudVarSystem_t {
    bool dirty;
    CanopyContext context;
    RedHash vars; // maps (char *varname) -> (STCloudVar var)
    RedHash dirty_vars; // maps (char *varname) -> void 
    RedHash callbacks; // maps (char *varname) -> (STOptions)
};

typedef struct STCloudVarBasicValue_t {
    union
    {
        bool val_bool;
        char * val_string;
        int8_t val_int8;
        uint8_t val_uint8;
        int16_t val_int16;
        uint16_t val_uint16;
        int32_t val_int32;
        uint32_t val_uint32;
        float val_float32;
        double val_float64;
        struct tm val_datetime;
    } val;
} STCloudVarBasicValue_t;

// STCloudVar_t is a recursive structure representing a cloud variable
// instance.
//
// Each node of this recursive structure contains pointers into another,
// similarly-structured recursive object: .decl (SDDL variable declaration).
//
// It is the responsibility of the routines in this module to keep these
// recursive structures and cross-references between them all consistent.
typedef struct STCloudVar_t 
{
    // Cloud variable system that owns this cloud variable.
    STCloudVarSystem sys;

    // Cloud variable's declaration: Recursive structure containing datatype,
    // qualifiers, and metadata for this cloud variable.
    SDDLVarDecl decl;

    // If cloud variable has a basic datatype, this holds its value.
    STCloudVarBasicValue_t * basic_value;

    // If cloud variable is an array, this holds its child elements.
    size_t array_num_items;
    STCloudVar *array_items;

    // If cloud variable is a struct, this holds its child members.
    // Hash Table: name --> STCloudVar
    RedHash struct_hash;

    // Has this cloud variable been touched since last sync?
    bool dirty;
} STCloudVar_t;

typedef struct STCloudVarValue_t {
    CanopyDatatypeEnum datatype;
    STCloudVarBasicValue_t basic_value;
    bool used;
} STCloudVarValue_t;

#endif // ST_CLOUDVAR_INTERNAL_INCLUDED
