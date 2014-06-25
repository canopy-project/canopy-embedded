/* Copyright 2014 - Greg Prisament
 */
#include "cano.h"
#include "canopy.h"

int RunProvision(int argc, const char *argv[])
{
    char * uuid = canopy_read_system_uuid();
    if (uuid)
        printf("Device UUID is %s\n", uuid);
    else
        printf("Could not determine device UUID\n");

    /*canopy_provision();*/
    return 0;
}
