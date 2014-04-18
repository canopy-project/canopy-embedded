#include "libwebsockets.h"
#include <stdio.h>

static int ws_callback(
        struct libwebsocket_context *this,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason,
        void *user,
        void *in,
        size_t len)
{
    switch (reason)
    {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLIENT_ESTABLISHED\n");
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
            break;
        case LWS_CALLBACK_CLOSED:
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLOSED\n");
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE:
            ((char *)in)[len] = '\0';
            fprintf(stderr, "rx %d '%s'\n", (int)len, (char *)in);
            break;
        /*case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:*/
        default:
            break;
    }
    return 0;
}

#define HAIVE_WS_PORT 1235
#define HAIVE_WS_USE_SSL 1
#define HAIVE_WS_ADDRESS "123-fake-st"
static struct libwebsocket_protocols sHaiveWsProtocols[] = {
    {
        "haive-ws-protocol",
        ws_callback,
        0,
        128
    },
    { NULL, NULL, 0, 0 } /* end */
};

int ws_main()
{
    struct lws_context_creation_info info;
    struct libwebsocket *wsi;
    struct libwebsocket_context *context;
    info.port = HAIVE_WS_PORT;
    info.protocols = sHaiveWsProtocols;
    info.gid = -1;
    info.uid = -1;

    context = libwebsocket_create_context(&info);
    if (!context)
    {
        fprintf(stderr, "Failed to create libwebsocket context\n");
        return 1;
    }

    wsi = libwebsocket_client_connect(
            context, 
            HAIVE_WS_ADDRESS, 
            HAIVE_WS_PORT, 
            HAIVE_WS_USE_SSL, 
            "/",
            0, /*?*/
            0, /*?*/
            "haive-ws-protocol",
            -1 /* latest ietf version */
        );
    if (!wsi)
    {
        fprintf(stderr, "Failed to create libwebsocket connection\n");
        return 1;
    }

    int result;
    while (result != 0)
    {
        result = libwebsocket_service(context, 0);
    }

    libwebsocket_context_destroy(context);
    return 0;
}
