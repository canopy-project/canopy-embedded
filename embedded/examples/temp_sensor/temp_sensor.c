#include <haive/haive.h>

float get_temp_celsius(void);

void handle_haive_event()
{
    switch (eventtype)
    {
        case HAIVE_EVENT_REPORT_REQUESTED:
        {
            HaiveReport report;
            report = have_begin_report(haive);
            haive_report_i32(report, "temp", get_temp_celsius());
            haive_send_report();
        }
    }
}


int main(void)
{
    HaiveContext haive;
    haive = haive_init();
    haive_load_device_description("/etc/haive/my_temp_sensor.sddl");
    haive_connect("ws://foobar");
    haive_register_callback(haive, HAIVE_CB_EVENT_HANDLER, handle_haive_event, NULL);
    haive_event_loop();
    haive_shutdown(haive);
    return 0;
}
