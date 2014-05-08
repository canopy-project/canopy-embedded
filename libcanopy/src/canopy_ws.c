#include "canopy.h"
#include "canopy_internal.h"
#include "libwebsockets.h"
#include <stdio.h>
#include <string.h>

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
            libwebsocket_callback_on_writable(this, wsi);
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
            break;
        case LWS_CALLBACK_CLOSED:
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLOSED\n");
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            char buf[LWS_SEND_BUFFER_PRE_PADDING + 6 + LWS_SEND_BUFFER_POST_PADDING];
            strcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], "hello");
            printf("Saying hi:\n");
            libwebsocket_write(wsi, (unsigned char *)&buf[LWS_SEND_BUFFER_PRE_PADDING], 6, LWS_WRITE_TEXT);
            libwebsocket_callback_on_writable(this, wsi);
            break;
        }
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

//#define CANOPY_WS_PORT 1235
#define CANOPY_WS_PORT CONTEXT_PORT_NO_LISTEN
#define CANOPY_WS_USE_SSL 0
#define CANOPY_WS_ADDRESS "canopy.link"
static struct libwebsocket_protocols sCanopyWsProtocols[] = {
    {
        "echo",
        ws_callback,
        1024,
        1024,
        0,
        NULL,
        0
    },
    { NULL, NULL, 0, 0, 0, NULL, 0} /* end */
};

int ws_main()
{
    struct lws_context_creation_info info={0};
    struct libwebsocket *wsi;
    struct libwebsocket_context *context;
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.iface = NULL;
    info.protocols = sCanopyWsProtocols;
    info.extensions = NULL;
    info.ssl_cert_filepath = NULL;
    info.ssl_private_key_filepath = NULL;
    info.ssl_ca_filepath = NULL;
    info.ssl_cipher_list = NULL;
    info.gid = -1;
    info.uid = -1;
    info.options = 0;
    info.user = NULL;
    info.ka_time = 0;
    info.ka_probes = 0;
    info.ka_interval = 0;

    //lws_set_log_level(511, NULL);

    context = libwebsocket_create_context(&info);
    if (!context)
    {
        fprintf(stderr, "Failed to create libwebsocket context\n");
        return 1;
    }

    wsi = libwebsocket_client_connect(
            context, 
            CANOPY_WS_ADDRESS, 
            8080, 
            CANOPY_WS_USE_SSL, 
            "/echo",
            "canopy.link", /*host?*/
            "http://gregprisament.com", /*origin?*/
            "echo",
            -1 /* latest ietf version */
        );
    if (!wsi)
    {
        fprintf(stderr, "Failed to create libwebsocket connection\n");
        return 1;
    }

    int result;
   // do
    //{
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
        result = libwebsocket_service(context, 800);
    //} while (result == 0);

        printf("Goodbye:\n");
    libwebsocket_context_destroy(context);
    return result;
}


bool canopy_set_cloud_host(CanopyContext canopy, const char *hostname)
{
    canopy->cloudHost = strdup(hostname);
    assert(canopy->cloudHost);
    return true;
}

bool canopy_set_cloud_port(CanopyContext canopy, uint16_t port)
{
    canopy->cloudPort = port;
    return true;
}

bool canopy_set_cloud_username(CanopyContext canopy, const char *username)
{
    canopy->cloudUsername = strdup(username);
    assert(canopy->cloudUsername);
    return true;
}

bool canopy_set_cloud_password(CanopyContext canopy, const char *password)
{
    canopy->cloudPassword = strdup(password);
    assert(canopy->cloudPassword);
    return true;
}

bool canopy_connect(CanopyContext canopy)
{
    ws_main();
    return true;
}
