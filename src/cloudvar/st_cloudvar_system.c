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

#include "cloudvar/st_cloudvar.h"
#include "cloudvar/st_cloudvar_internal.h"

STCloudVarSystem st_cloudvar_system_new(CanopyContext ctx)
{
    STCloudVarSystem sys;

    sys = calloc(1, sizeof(struct STCloudVarSystem_t));
    sys->dirty = true;
    sys->context = ctx;
    sys->vars = RedHash_New(0);
    sys->dirty_vars = RedHash_New(0);
    sys->callbacks = RedHash_New(0);
    return sys;
}

void st_cloudvar_system_free(STCloudVarSystem sys)
{
    if (sys)
    {
        // TODO: free all entries in hash table
        //RedHash_Free(sys->vars);
        //RedHash_Free(sys->dirty_vars);
        free(sys);
    }
}

bool st_cloudvar_system_contains(STCloudVarSystem sys, const char *varname)
{
    return RedHash_HasKeyS(sys->vars, varname);
}

void st_cloudvar_system_clear_dirty(STCloudVarSystem sys)
{
    sys->dirty = false;
    RedHash_Clear(sys->dirty_vars);
}

void st_cloudvar_system_mark_dirty(STCloudVarSystem sys, STCloudVar var)
{
    const char *name = st_cloudvar_name(var);
    RedHash_UpdateOrInsertS(sys->dirty_vars, NULL, name, (void *)true);
    sys->dirty = true;
}

bool st_cloudvar_system_is_dirty(STCloudVarSystem sys)
{
    return sys->dirty;
}

uint32_t st_cloudvar_system_num_dirty(STCloudVarSystem sys)
{
    return RedHash_NumItems(sys->dirty_vars);
}

STCloudVar st_cloudvar_system_lookup_var(STCloudVarSystem sys, const char *varname)
{
    return RedHash_GetWithDefaultS(sys->vars, varname, NULL);
}

STCloudVar st_cloudvar_system_dirty_var(STCloudVarSystem sys, uint32_t idx)
{
    // TODO: Inefficient!
    RedHashIterator_t iter;
    const void *key;
    size_t keySize;
    uint32_t i = 0;
    RED_HASH_FOREACH(iter, sys->dirty_vars, &key, &keySize, NULL)
    {
        const char *varname = (const char *)key;
        if (i == idx)
        {
            return RedHash_GetWithDefaultS(sys->vars, varname, NULL);
        }
        i++;
    }
    return NULL;
}

