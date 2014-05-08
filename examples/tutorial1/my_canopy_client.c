#include <canopy.h>
#include <sys/reboot.h>

#define CANOPY_CLOUD_HOST
#define CANOPY_CLOUD_PORT
#define CANOPY_CLOUD_USERNAME "myusername"
#define CANOPY_CLOUD_PASSWORD "mypassword"

static float get_cpu_usage()
{
    FILE *fp;
    float val;
    fp = fopen("/proc/loadavg");
    fscanf(fp, "%f", &val);
    fclose(fp);
    return val;
}

static void handle_canopy_event(CanopyContext ctx, CanopyEventDetails event)
{
    switch (canopy_get_event_type(event))
    {
        case CANOPY_EVENT_REPORT_REQUESTED:
        {
            CanopyReport report;
            float cpu = get_cpu_usage();

            report = canopy_begin_report(ctx);
            canopy_report_float32(report, "cpu", cpu);
            canopy_send_report(ctx);
            break;
        }
        case CANOPY_EVENT_CONTROL_TRIGGER:
        {
            if (canopy_event_control_name_is(event, "reboot"))
            {
                reboot(RB_AUTOBOOT);
                break;
            }
        }
    }
}

int main()
{
    CanopyContext ctx;
    ctx = canopy_init();
    canopy_load_device_description(ctx, "tutorial.sddl", "canopy.tutorial.sample_device_1");
    canopy_connect(ctx, CANOPY_CLOUD_HOST, CANOPY_CLOUD_PORT, CANOPY_CLOUD_USERNAME, CANOPY_CLOUD_PASSWORD);
    canopy_register_callback(ctx, CANOPY_CB_EVENT_HANDLER, handle_canopy_event, NULL);
    canopy_event_loop(ctx);
    canopy_shutdown(ctx);
    return 0;
}
