#include "canopy.h"
#include "canopy_internal.h"
#include "red_log.h"
#include <stdio.h>
#include <string.h>

void _canopy_ws_write(CanopyContext canopy, const char *msg)
{
    char *buf;
    if (!canopy->ws_write_ready)
    {
        RedLog_DebugLog("canopy", "WS not ready for write!  Skipping.");
        return;
    }
    size_t len = strlen(msg);
    buf = calloc(1, LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING);
    strcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], msg);
    libwebsocket_write(canopy->ws, (unsigned char *)&buf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);
    canopy->ws_write_ready = false;
    libwebsocket_callback_on_writable(canopy->ws_ctx, canopy->ws);
    free(buf);
}

static int ws_callback(
        struct libwebsocket_context *this,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason,
        void *user,
        void *in,
        size_t len)
{
    CanopyContext canopy = (CanopyContext)libwebsocket_context_user(this);
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
            canopy->ws_write_ready = true;
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

bool canopy_set_auto_reconnect(CanopyContext canopy, bool enabled)
{
    canopy->autoReconnect = enabled;
    return true;
}

bool canopy_connect(CanopyContext canopy)
{
    struct lws_context_creation_info info={0};
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
    info.user = canopy;
    info.ka_time = 0;
    info.ka_probes = 0;
    info.ka_interval = 0;

    //lws_set_log_level(511, NULL);

    canopy->ws_ctx = libwebsocket_create_context(&info);
    if (!canopy->ws_ctx)
    {
        fprintf(stderr, "Failed to create libwebsocket context\n");
        return 1;
    }

    canopy->ws = libwebsocket_client_connect(
            canopy->ws_ctx, 
            CANOPY_WS_ADDRESS, 
            8080, 
            CANOPY_WS_USE_SSL, 
            "/echo",
            "canopy.link", /*host?*/
            "http://gregprisament.com", /*origin?*/
            "echo",
            -1 /* latest ietf version */
        );
    if (!canopy->ws)
    {
        fprintf(stderr, "Failed to create libwebsocket connection\n");
        return 1;
    }

    return true;
}
