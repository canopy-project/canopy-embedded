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
#include "red_uuid.h"
#include <canopy.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

int RunUUID(int argc, const char *argv[])
{
    char *uuid = canopy_read_system_uuid();
    if (uuid)
    {
        printf("Device UUID is: %s\n", uuid);
    }
    else if (argc == 2 || (argc > 2 && strcmp(argv[2], "--install")))
    {
        printf("Device has not been assigned a UUID.\n");
        printf("Run \"sudo cano uuid --install\"\n");
    }
    else if (argc > 2 && !strcmp(argv[2], "--install"))
    {
        RedUUID_t uuid;
        FILE *fp;
        char *uuidString;
        uuid = RedUUID_GenV4();
        uuidString = RedUUID_ToNewString(uuid);
        printf("New UUID: %s\n", uuidString);
        mkdir("/etc/canopy/", 755);
        fp = fopen("/etc/canopy/uuid", "w+");
        if (!fp)
        {
            printf("Failed to write to /etc/canopy/uuid.  Make sure you use sudo\n");
            return 1;
        }
        fprintf(fp, "%s\n", uuidString);
        fclose(fp);
        free(uuidString);
        printf("UUID written to: /etc/canopy/uuid\n");
    }
    return 0;
}
