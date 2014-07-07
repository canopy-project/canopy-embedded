/* Copyright 2014 - Greg Prisament
 */
#include "cano.h"
#include "canopy.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>

int RunProvision(int argc, const char *argv[])
{
    CanopyContext canopy = canopy_init();
    struct termios old, new;
    CanopyProvisionResults results;
    const char *uuid;
    FILE *fp;
    char username[256]; /* TODO: buff overrun possibility */
    char password[256];

    /* Get username */
    printf("\nUsing cloud server: %s://%s:%d\n",
            canopy_get_web_protocol(canopy),
            canopy_get_cloud_host(canopy),
            canopy_get_cloud_port(canopy));
    printf("Cloud Account Username: ");
    fflush(stdout);
    scanf("%255s", username);

    printf("Cloud Account Password: ");
    /* Turn echoing off and fail if we can't. */
    if (tcgetattr (STDIN_FILENO, &old) != 0)
        return -1;
    new = old;
    new.c_lflag &= ~ECHO;
    if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &new) != 0)
        return -1;

    fflush(stdout);
    scanf("%255s", password);
    /* TODO: clear asap */

    /* Restore terminal. */
    (void) tcsetattr (STDIN_FILENO, TCSAFLUSH, &old);

    printf("\n");

    if (!canopy)
    {
        printf("canopy_init failed\n");
        return 1;
    }
    results = canopy_provision(canopy, username, password);
    if (!results)
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
    printf("UUID %s written to: /etc/canopy/uuid\n", uuid);
    free(results);

    return 0;
}
