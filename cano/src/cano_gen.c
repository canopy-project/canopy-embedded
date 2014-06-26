/* Copyright 2014 - Greg Prisament
 */
#include "sddl.h"
#include "red_string.h"
#include <stdio.h>
#include "canopy_boilerplate_head.out.h"
#include "canopy_boilerplate_tail.out.h"
#include <string.h>

static const char *_get_op_name(SDDLControlTypeEnum controlType)
{
    switch (controlType)
    {
        case SDDL_CONTROL_TYPE_PARAMETER:
            return "change";
        case SDDL_CONTROL_TYPE_TRIGGER:
            return "trigger";
        default:
            return NULL;
    }
}
static const char *_get_control_c_type(SDDLDatatypeEnum datatype)
{
    switch (datatype)
    {
        case SDDL_DATATYPE_VOID:
            return "void";
        case SDDL_DATATYPE_STRING:
            return "char *";
        case SDDL_DATATYPE_BOOL:
            return "bool";
        case SDDL_DATATYPE_INT8:
            return "int8_t";
        case SDDL_DATATYPE_UINT8:
            return "uint8_t";
        case SDDL_DATATYPE_INT16:
            return "int16_t";
        case SDDL_DATATYPE_UINT16:
            return "uint16_t";
        case SDDL_DATATYPE_INT32:
            return "int32_t";
        case SDDL_DATATYPE_UINT32:
            return "uint32_t";
        case SDDL_DATATYPE_FLOAT32:
            return "float";
        case SDDL_DATATYPE_FLOAT64:
            return "double";
        case SDDL_DATATYPE_DATETIME:
            return "datetime";
        default:
            return NULL;
    }
}
static const char *_get_control_abbrev_type(SDDLDatatypeEnum datatype)
{
    switch (datatype)
    {
        case SDDL_DATATYPE_VOID:
            return "void";
        case SDDL_DATATYPE_STRING:
            return "string";
        case SDDL_DATATYPE_BOOL:
            return "bool";
        case SDDL_DATATYPE_INT8:
            return "i8";
        case SDDL_DATATYPE_UINT8:
            return "u8";
        case SDDL_DATATYPE_INT16:
            return "i16";
        case SDDL_DATATYPE_UINT16:
            return "u16";
        case SDDL_DATATYPE_INT32:
            return "i32";
        case SDDL_DATATYPE_UINT32:
            return "u32";
        case SDDL_DATATYPE_FLOAT32:
            return "float32";
        case SDDL_DATATYPE_FLOAT64:
            return "float64";
        case SDDL_DATATYPE_DATETIME:
            return "datetime";
        default:
            return NULL;
    }
}

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
        fprintf(stderr, "Could not open %s.h for write!\n", className);
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
            SDDLControl control = SDDL_CONTROL(prop);
            SDDLDatatypeEnum datatype = sddl_control_datatype(control);
            SDDLControlTypeEnum controlType = sddl_control_type(control);
            const char *controlName = sddl_control_name(control);
            const char *controlCType = _get_control_c_type(datatype);
            const char *opName = _get_op_name(controlType);
            if (!controlCType)
            {
                fprintf(stderr, "Datatype does not have C type\n");
                return false;
            }

            if (datatype == SDDL_DATATYPE_VOID)
            {
                fprintf(fp, "static bool on_%s__%s(CanopyContext canopy);\n",
                        opName,
                        controlName);
            }
            else
            {
                fprintf(fp, "static bool on_%s__%s(CanopyContext canopy, %s value);\n",
                        opName,
                        controlName,
                        controlCType);
            }
        }
    }

    /* Implement dispatch reoutine */
    fprintf(fp, "static void dispatch(CanopyEventDetails event)\n{\n");
    fprintf(fp, "    CanopyContext ctx = canopy_event_context(event);\n");

    numProperties = sddl_class_num_properties(cls);
    for (i = 0; i < numProperties; i++)
    {
        SDDLProperty prop;
        prop = sddl_class_property(cls, i);
        if (sddl_is_control(prop))
        {
            SDDLControl control = SDDL_CONTROL(prop);
            SDDLDatatypeEnum datatype = sddl_control_datatype(control);
            SDDLControlTypeEnum controlType = sddl_control_type(control);
            const char *controlName = sddl_control_name(control);
            const char *controlCType = _get_control_c_type(datatype);
            const char *controlAbbrevType = _get_control_abbrev_type(datatype);
            const char *opName = _get_op_name(controlType);
            if (!controlCType || !controlAbbrevType)
            {
                fprintf(stderr, "Datatype does not have C type or abbrev type\n");
                return false;
            }

            if (datatype == SDDL_DATATYPE_VOID)
            {
                fprintf(fp,
                    "    if (canopy_event_control_name_matches(event, \"%s\"))\n"
                    "    {\n"
                    "        on_%s__%s(ctx);\n"
                    "    }\n",
                    controlName,
                    opName,
                    controlName);
            }
            else
            {
                fprintf(fp,
                    "    if (canopy_event_control_name_matches(event, \"%s\"))\n"
                    "    {\n"
                    "        %s val;\n"
                    "        canopy_event_get_control_value_%s(event, &val);\n"
                    "        on_%s__%s(ctx, val);\n"
                    "    }\n",
                    controlName,
                    controlCType,
                    controlAbbrevType,
                    opName,
                    controlName);
            }
        }
    }
    fprintf(fp, "}\n\n");

    for (i = 0; i < sizeof(_CANOPY_BOILERPLATE_TAIL)/sizeof(_CANOPY_BOILERPLATE_TAIL[0]); i++)
    {
        fprintf(fp, "%s\n", _CANOPY_BOILERPLATE_TAIL[i]);
    }
    return true;
}

static bool _dump_class_control_callbacks(SDDLClass cls, const char *classShortName)
{
    FILE *fp;
    unsigned numProperties, i;
    char filename[512];
    snprintf(filename, 512, "%s.c", classShortName);
    fp = fopen(filename, "w+");
    if (!fp)
    {
        fprintf(stderr, "Could not open stubz.h for write!");
        return false;
    }
    snprintf(filename, 512, "%s.h", classShortName);
    fprintf(fp, "#include \"%s\"\n", filename);
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
            SDDLControl control = SDDL_CONTROL(prop);
            SDDLDatatypeEnum datatype = sddl_control_datatype(control);
            SDDLControlTypeEnum controlType = sddl_control_type(control);
            const char *controlName = sddl_control_name(control);
            const char *controlCType = _get_control_c_type(datatype);
            const char *opName = _get_op_name(controlType);
            if (datatype == SDDL_DATATYPE_VOID)
            {
                fprintf(fp, "static bool on_%s__%s(CanopyContext canopy)\n"
                    "{\n"
                    "   /* Your code here.\n"
                    "    * Return true on success.\n"
                    "    */\n"
                    "   return false;\n}\n\n", 
                    opName,
                    controlName);
            }
            else
            {
                fprintf(fp, "static bool on_%s__%s(CanopyContext canopy, %s value)\n"
                    "{\n"
                    "   /* Your code here.\n"
                    "    * Return true on success.\n"
                    "    */\n"
                    "   return false;\n}\n\n", 
                    opName,
                    controlName,
                    controlCType);
            }
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

static bool _dump_makefile(RedStringList cFilenames)
{
    unsigned i;
    RedStringList targets = RedStringList_New();

    RedString tmp = RedString_New(NULL);
    for (i = 0; i < RedStringList_NumStrings(cFilenames); i++)
    {
        RedString_SubString(tmp, RedStringList_GetString(cFilenames, i), 0, -3);
        RedStringList_AppendChars(targets, RedString_GetChars(tmp));
    }
    RedString_Free(tmp);

    FILE *fp = fopen("makefile", "w+");
    if (!fp)
    {
        fprintf(stderr, "Could not open makefile for write\n");
        return false;
    }
    
    fprintf(fp, "TARGETS := ");
    for (i = 0; i < RedStringList_NumStrings(cFilenames); i++)
    {
        fprintf(fp, "%s ", RedString_GetChars(RedStringList_GetString(targets, i)));
    }
    fprintf(fp, "\n");
    fprintf(fp, ".PHONY: default\n");
    fprintf(fp, "default: $(TARGETS)\n");
    for (i = 0; i < RedStringList_NumStrings(cFilenames); i++)
    {
        fprintf(fp, "%s: %s\n", 
                RedString_GetChars(RedStringList_GetString(targets, i)), 
                RedString_GetChars(RedStringList_GetString(cFilenames, i)));
        fprintf(fp, "\tgcc $< -g -lcanopy -lwebsockets -o $@\n");
    }
    fclose(fp);
    RedStringList_Free(targets);
    return true;
}

static const char *_find_char_from_right(const char*in, char ch)
{
    int len = strlen(in);
    int i;
    for (i = 0; i < len; i++)
    {
        if (in[len-i-1] == ch)
        {
            return &in[len-i];
        }
    }
    return in;
}

int RunGen(int argc, const char *argv[])
{
    SDDLDocument doc;
    unsigned numProperties, i;
    RedStringList cFilenames = RedStringList_New();

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
            name = _find_char_from_right(name, '.');
            if ((argc > 3 && !strcmp(name, argv[3])) || argc == 3)
            {
                RedStringList_AppendPrintf(cFilenames, "%s.c", name);
                _dump_boilerplate(SDDL_CLASS(prop), argv[2], name);
                _dump_class_control_callbacks(SDDL_CLASS(prop), name);
            }
        }
    }
    _dump_makefile(cFilenames);
    RedStringList_Free(cFilenames);
    return 0;
}
