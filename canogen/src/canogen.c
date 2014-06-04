#include "sddl.h"
#include "canopy_boilerplate_head.out.h"
#include <string.h>


static bool _dump_boilerplate()
{
    FILE *fp;
    int i;
    fp = fopen("canopy_boilerplate.h", "w+");
    if (!fp)
    {
        fprintf(stderr, "Could not open canopy_boilerplate.h for write!");
        return false;
    }
    for (i = 0; i < sizeof(_CANOPY_BOILERPLATE_HEAD)/sizeof(_CANOPY_BOILERPLATE_HEAD[0]); i++)
    {
        fprintf(fp, "%s\n", _CANOPY_BOILERPLATE_HEAD[i]);
    }
    return true;
}

static bool _dump_class_control_callbacks(SDDLClass cls)
{
    FILE *fp;
    unsigned numProperties, i;
    fp = fopen("stubz.c", "w+");
    if (!fp)
    {
        fprintf(stderr, "Could not open stubz.h for write!");
        return false;
    }
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
            fprintf(fp, "bool on_change__%s(uint32_t value)\n{\n}\n\n", propName);
        }
    }
    fprintf(fp, "bool on_canopy_init()\n{\n}\n\n");
    fprintf(fp, "bool on_canopy_shutdown()\n{\n}\n\n");
    fprintf(fp, "bool on_connected()\n{\n}\n\n");
    fprintf(fp, "bool on_disconnected()\n{\n}\n\n");
    fprintf(fp, "bool on_report_requested()\n{\n}\n\n");
    fclose(fp);
    return true;
}

int main(int argc, const char *argv[])
{
    SDDLDocument doc;
    unsigned numProperties, i;
    doc = sddl_load_and_parse(argv[1]);

    _dump_boilerplate();

    numProperties = sddl_document_num_properties(doc);
    for (i = 0; i < numProperties; i++)
    {
        SDDLProperty prop;
        prop = sddl_document_property(doc, i);
        if (sddl_is_class(prop))
        {
            const char *name;
            name = sddl_class_name(SDDL_CLASS(prop));
            if ((argc > 2 && !strcmp(name, argv[2]) )|| argc <= 2)
            {
                _dump_class_control_callbacks(SDDL_CLASS(prop));
            }
        }
    }
    return true;
}
