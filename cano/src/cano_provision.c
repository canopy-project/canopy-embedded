/* Copyright 2014 - Greg Prisament
 */
#include "cano.h"
#include "canopy.h"

int RunProvision(int argc, const char *argv[])
{
    CanopyContext canopy = canopy_init();
    CanopyProvisionResults result;
    bool result;
    FILE *fp;

    if (!canopy)
    {
        printf("canopy_init failed\n");
        return 1;
    }
    results = canopy_provision(canopy, "greg", "1234");
    if (!result)
    {
        printf("Provisioning failed!\n");
        return 1;
    }
    uuid = canopy_provision_get_uuid(results);

    /* Write UUID to /etc/canopy/uuid */
    mkdir("/etc/canopy/", 755);
    fp = fopen("/etc/canopy/uuid", "w+");
    if (!fp)
    {
        printf("Failed to write to /etc/canopy/uuid.  Make sure you use sudo\n");
        return 1;
    }
    fprintf(fp, "%s\n", uuid);
    fclose(fp);
    printf("UUID written to: /etc/canopy/uuid\n");
    free(results);

    return 0;
}
