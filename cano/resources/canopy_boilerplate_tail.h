
static bool handle_canopy_event(CanopyContext ctx, CanopyEventDetails event)
{
    switch (canopy_get_event_type(event))
    {
        case CANOPY_EVENT_CONNECTION_ESTABLISHED:
        {
            on_connected(ctx);
            break;
        }
        case CANOPY_EVENT_CONNECTION_LOST:
        {
            on_disconnected(ctx);
            break;
        }
        case CANOPY_EVENT_REPORT_REQUESTED:
        {
            on_report_requested(ctx);
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

    ctx = canopy_init();
    if (!ctx)
    {
        fprintf(stderr, "Failed to initialize canopy\n");
        return false;
    }
    on_canopy_init(ctx);
    canopy_load_sddl(ctx, SDDL_FILENAME, SDDL_CLASSNAME);

    canopy_register_event_callback(ctx, handle_canopy_event, NULL);

    canopy_set_auto_reconnect(ctx, true);
    canopy_connect(ctx);

    canopy_event_loop(ctx);
    on_canopy_shutdown(ctx);
    canopy_shutdown(ctx);
    return 0;
}

#endif
