
static bool handle_canopy_event(CanopyContext ctx, CanopyEventDetails event)
{
    switch (canopy_get_event_type(event))
    {
        case CANOPY_EVENT_CONNECTION_ESTABLISHED:
        {
            on_connected();
            break;
        }
        case CANOPY_EVENT_CONNECTION_LOST:
        {
            on_disconnected();
            break;
        }
        case CANOPY_EVENT_REPORT_REQUESTED:
        {
            on_report_requested();
            break;
        }
        case CANOPY_EVENT_CONTROL_TRIGGER:
        {
            dispatch(event);
        }
        default:
        {
            break;
        }
    }
    return true;
}

int main(int argc, const char *argv[])
{
    CanopyContext ctx;
    if (argc < 2)
    {
        printf("USAGE: %s <DEVICE_ID>\n", argv[0]);
        return -1;
    }
    ctx = canopy_init();
    on_canopy_init();
    canopy_load_device_description(ctx, "tutorial.sddl", "canopy.tutorial.sample_device_1");

    canopy_register_event_callback(ctx, handle_canopy_event, NULL);

    canopy_set_cloud_host(ctx, CANOPY_CLOUD_HOST);
    canopy_set_cloud_port(ctx, CANOPY_CLOUD_PORT);
    canopy_set_cloud_username(ctx, CANOPY_CLOUD_USERNAME);
    canopy_set_cloud_password(ctx, CANOPY_CLOUD_PASSWORD);
    canopy_set_device_id(ctx, argv[1]);
    canopy_set_auto_reconnect(ctx, true);
    canopy_connect(ctx);

    canopy_event_loop(ctx);
    canopy_shutdown(ctx);
    on_canopy_shutdown();
    return 0;
}

#endif
