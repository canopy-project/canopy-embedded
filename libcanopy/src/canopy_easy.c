/*
 * Copyright 2014 Gregory Prisament
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
Run with:
gcc --std=c99 -I../include -I../3rdparty/libred/include -I../3rdparty/libred/under_construction canopy_easy.c ../3rdparty/libred/src/red_string.c && ./a.out
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "red_string.h"
#include "canopy_internal.h"

static void ciutil_post(const char *url, const char *payload)
{
    printf("POSTING TO %s\n", url);
    printf("%s\n", payload);
}

bool canopy_easy_post_sample(void * params, ...)
{
    va_list ap;
    bool done=false;
    void * param;
    va_start(ap, params);
    char * cloud_host = "ccs.canopy.link";
    char * device_uuid = NULL;
    char * property_name = NULL;
    char * url;
    char * payload;
    _CanopyPropertyValue value = {0};

    /* Read arguments */
    param = params;
    while (!done)
    {
        if (param == NULL)
            done = true;
        else
        {
            switch ((uintptr_t)param)
            {
                case (uintptr_t)CANOPY_CLOUD_HOST:
                {
                    cloud_host = va_arg(ap, char *);
                    break;
                }
                case (uintptr_t)CANOPY_DEVICE_ID:
                {
                    device_uuid = va_arg(ap, char *);
                    break;
                }
                case (uintptr_t)CANOPY_PROPERTY_NAME:
                {
                    property_name = va_arg(ap, char *);
                    break;
                }
                case (uintptr_t)CANOPY_VALUE_FLOAT32:
                {
                    float value_float32 = (float)va_arg(ap, double);
                    value.datatype = SDDL_DATATYPE_FLOAT32;
                    value.val.val_float32 = value_float32;
                    break;
                }
                default:
                {
                    fprintf(stderr, "Invalid argument to canopy_easy_post_sample.  Did you forget to end with NULL?\n");
                }
            }
        }

        param = va_arg(ap, void *);
    }

    /* Validate arguments */
    if (property_name == NULL)
    {
        fprintf(stderr, "Property name expected\n");
        return false;
    }
    if (value.datatype == SDDL_DATATYPE_INVALID)
    {
        fprintf(stderr, "CANOPY_VALUE_??? expected\n");
        return false;
    }

    /* Construct url & payload */
    url = RedString_PrintfToNewChars("http://%s/api/devices/%s", cloud_host, device_uuid);
    payload = RedString_PrintfToNewChars("{ \"%s\" : %f }", property_name, value.val.val_float32);

    /* Post */
    ciutil_post(url, payload);

    printf("hostname: %s\n", cloud_host);
    printf("device_uuid: %s\n", device_uuid);
    printf("property: %s\n", property_name);

    va_end(ap);
    return false;
}

int main(void)
{
    canopy_easy_post_sample(
            CANOPY_CLOUD_HOST, "canopy.link", 
            CANOPY_DEVICE_ID, "16eeca6a-e8dc-4c54-b78e-6a7416803ca8",
            CANOPY_PROPERTY_NAME, "temperature",
            CANOPY_VALUE_FLOAT32, 4.0f,
            NULL);
    return 0;
}

#if 0
bool canopy_post_sample(params, ...)
{
    va_list ap;
    const char *host = "https://canopy.link";
    const char *device_uuid = NULL;
    ZARRAY(char *) prop_names;
    ZARRAY(void *) prop_values;
    uint32_t param;

    va_start(ap, params);

    while (!done)
    {
        param = va_arg(params, uint32_t);
        switch (param)
        {
            case CANOPY_CLOUD_HOST:
                host = va_arg(params, const char *);
                break;
            case CANOPY_DEVICE_ID:
                device_uuid = va_arg(params, const char *);
                break;
            case CANOPY_PROPERTY_NAME:
                prop_name = va_arg(params, const char *);
                ZARRAY_APPEND(prop_names, prop_name);
            case CANOPY_VALUE_FLOAT32:
                ZARRAY_APPEND(prop_values, value);
            case 0:
                done = true;
            default:
                /* Invalid argument.  Did you forget NULL? */
                return false;
        }
    }

    response = ciutil_post(url, payload);
    if (!response) {
        return false;
    }
    return true;
}
#endif
