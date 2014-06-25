/* Copyright 2014 - Greg Prisament
 */
#include "sddl.h"
#include <stdio.h>
#include "canopy_boilerplate_head.out.h"
#include "canopy_boilerplate_tail.out.h"
#include <string.h>


static bool _dump_boilerplate(SDDLClass cls, const char *sddlFilename, const char *className)
{
    FILE *fp;
    int i;
    int numProperties;
    char filename[512];
    snprintf(filename, 512, "%s.h", className);
    fp = fopen(filename, "w+");
    if (!fp)
    {
        fprintf(stderr, "Could not open %s.h for write!", className);
        return false;
    }
    for (i = 0; i < sizeof(_CANOPY_BOILERPLATE_HEAD)/sizeof(_CANOPY_BOILERPLATE_HEAD[0]); i++)
    {
        fprintf(fp, "%s\n", _CANOPY_BOILERPLATE_HEAD[i]);
    }

    fprintf(fp, "#define SDDL_FILENAME \"%s\"\n", sddlFilename);
    fprintf(fp, "#define SDDL_CLASSNAME \"%s\"\n", className);

    /* Control callback prototypes */
    numProperties = sddl_class_num_properties(cls);
    for (i = 0; i < numProperties; i++)
    {
        SDDLProperty prop;
        prop = sddl_class_property(cls, i);
        if (sddl_is_control(prop))
        {
            const char *controlName = sddl_control_name(SDDL_CONTROL(prop));
            const char *controlCType = "int8_t";

            fprintf(fp, "static bool on_change__%s(CanopyContext canopy, %s value);\n",
                    controlName,
                    controlCType);
        }
    }

    /* Implement dispatch reoutine */
    fprintf(fp, "static void dispatch(CanopyEventDetails event)\n{\n");
    fprintf(fp, "    CanopyContext ctx = canopy_event_context(event);");

    numProperties = sddl_class_num_properties(cls);
    for (i = 0; i < numProperties; i++)
    {
        SDDLProperty prop;
        prop = sddl_class_property(cls, i);
        if (sddl_is_control(prop))
        {
            const char *controlName = sddl_control_name(SDDL_CONTROL(prop));
            const char *controlCType = "int8_t";
            const char *controlAbbrevType = "i8";

            fprintf(fp,
                "    if (canopy_event_control_name_matches(event, \"%s\"))\n"
                "    {\n"
                "        %s val;\n"
                "        canopy_event_get_control_value_%s(event, &val);\n"
                "        on_change__%s(ctx, val);\n"
                "    }\n",
                controlName,
                controlCType,
                controlAbbrevType,
                controlName);
        }
    }
    fprintf(fp, "}\n\n");

    for (i = 0; i < sizeof(_CANOPY_BOILERPLATE_TAIL)/sizeof(_CANOPY_BOILERPLATE_TAIL[0]); i++)
    {
        fprintf(fp, "%s\n", _CANOPY_BOILERPLATE_TAIL[i]);
    }
    return true;
}

static bool _dump_class_control_callbacks(SDDLClass cls)
{
    FILE *fp;
    unsigned numProperties, i;
    char filename[512];
    snprintf(filename, 512, "%s.c", sddl_class_name(cls));
    fp = fopen(filename, "w+");
    if (!fp)
    {
        fprintf(stderr, "Could not open stubz.h for write!");
        return false;
    }
    snprintf(filename, 512, "%s.h", sddl_class_name(cls));
    fprintf(fp, "#include \"%s.h\"\n", filename);
    fprintf(fp, "#include <canopy.h>\n");
    fprintf(fp, "\n");
    numProperties = sddl_class_num_properties(cls);
    for (i = 0; i < numProperties; i++)
    {
        SDDLProperty prop;
        prop = sddl_class_property(cls, i);
        if (sddl_is_class(prop))
        {
            /*_dump_control_callbacks_recur();*/
        }
        else if (sddl_is_control(prop))
        {
            const char *propName = sddl_control_name(SDDL_CONTROL(prop));
            fprintf(fp, "static bool on_change__%s(CanopyContext canopy, int8_t value)\n"
                "{\n"
                "   /* Your code here.\n"
                "    * Return true on success.\n"
                "    */\n"
                "   return false;\n}\n\n", propName);
        }
    }
    fprintf(fp, "static bool on_canopy_init(CanopyContext canopy)\n{\n    return false;\n}\n\n");
    fprintf(fp, "static bool on_canopy_shutdown(CanopyContext canopy)\n{\n    return false;\n}\n\n");
    fprintf(fp, "static bool on_connected(CanopyContext canopy)\n{\n    return false;\n}\n\n");
    fprintf(fp, "static bool on_disconnected(CanopyContext canopy)\n{\n    return false;\n}\n\n");
    fprintf(fp, "static bool on_report_requested(CanopyContext canopy)\n{\n    return false;\n}\n\n");
    fclose(fp);
    return true;
}


int RunGen(int argc, const char *argv[])
{
    SDDLDocument doc;
    unsigned numProperties, i;

    if (argc == 2)
    {
        printf("usage: cano gen <sddl_filename> [<args>]\n");
        return -1;
    }

    doc = sddl_load_and_parse(argv[2]);
    if (!doc)
    {
        printf("fatal: error loading SDDL file\n");
        return -1;
    }

    numProperties = sddl_document_num_properties(doc);
    for (i = 0; i < numProperties; i++)
    {
        SDDLProperty prop;
        prop = sddl_document_property(doc, i);
        if (sddl_is_class(prop))
        {
            const char *name;
            name = sddl_class_name(SDDL_CLASS(prop));
            if ((argc > 3 && !strcmp(name, argv[3])) || argc == 3)
            {
                const char *className = sddl_class_name(SDDL_CLASS(prop));
                _dump_boilerplate(SDDL_CLASS(prop), argv[2], className);
                _dump_class_control_callbacks(SDDL_CLASS(prop));
            }
        }
    }
    return 0;
}
