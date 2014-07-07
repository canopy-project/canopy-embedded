/* Copyright 2014 - Greg Prisament
 */
#include <canopy.h>
#include <stdio.h>

int RunInfo(int argc, const char *argv[])
{
    printf("libcanopy version: 0.01 pre-alpha devel\n");
    printf("cano tool version: 0.01 pre-alpha devel\n");
    printf("\n");
    printf("SYSCONFIGDIR: %s", canopy_get_sysconfigdir());
    return 0;
}
