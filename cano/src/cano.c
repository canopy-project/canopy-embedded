/*
 * test -- Runs tests.
 *
 * gen -- Generates embedded code.
 *
 * provision -- Associates this device with a Canopy cloud account.
 *
 * help -- Shows this help file
 */
#include "cano.h"
#include <stdio.h>
#include <string.h>

static int PrintUsage()
{
    printf("usage: cano <command> [<args>]\n\n");
    printf("cano is a tool for embedded development with Canopy.\n");
    printf("It supports the following commands:\n\n");

    printf("  gen        -- Auto-generate embedded code based on SDDL file\n");
    printf("  help       -- Get general help\n");
    printf("  help <CMD> -- Get help for a specific command\n");
    printf("  info       -- Show info about libcanopy installation\n");
    printf("  provision  -- Grant cloud account access to this device\n");
    printf("  test       -- Run test suite\n");
    printf("  uuid       -- Generate and configure device's UUID\n");
    printf("\n");
    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc == 1)
    {
        PrintUsage();
        return 0;
    }
    
    if (!strcmp(argv[1], "test"))
    {
        return RunTest(argc, argv);
    }
    else if (!strcmp(argv[1], "gen"))
    {
        return RunGen(argc, argv);
    }
    else if (!strcmp(argv[1], "provision"))
    {
        return RunProvision(argc, argv);
    }
    else if (!strcmp(argv[1], "help"))
    {
        return PrintUsage();
    }
    else if (!strcmp(argv[1], "info"))
    {
        return RunInfo(argc, argv);
    }
    else if (!strcmp(argv[1], "uuid"))
    {
        return RunUUID(argc, argv);
    }
    else
    {
        PrintUsage();
        printf("fatal: Unknown command %s\n", argv[1]);
        return 0;
    }
    return 0;
}
