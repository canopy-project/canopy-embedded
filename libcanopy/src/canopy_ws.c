#include "canopy.h"
#include "canopy_internal.h"
#include "red_log.h"
#include "red_json.h"
#include "sddl.h"
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

static void _process_ws_payload(CanopyContext canopy, const char *payload)
{
    /* Payload: 
     * {
     *    "control" : {
     *      "speed" : 2
     *    }
     * }
     */
    RedJsonObject jsonObj = RedJson_Parse(payload);
    if (!jsonObj)
    {
        RedLog_WarnLog("canopy", "JSON parsing of WS payload failed!");
        return;
    }

    //if (RedJsonObject_HasKey(jsonObj, "control"))
    if (RedJsonObject_HasKey(jsonObj, "Data")) // fan demo hack
    {
        //if (RedJsonObject_IsValueObject(jsonObj, "control"))
        if (RedJsonObject_IsValueObject(jsonObj, "Data")) // fan demo hack
        {
            //RedJsonObject controlObj = RedJsonObject_GetObject(jsonObj, "control");
            RedJsonObject controlObj = RedJsonObject_GetObject(jsonObj, "Data"); // fan demo hack
            unsigned numKeys = RedJsonObject_NumItems(controlObj);
            unsigned i;
            char ** keysArray = RedJsonObject_NewKeysArray(controlObj);
            for (i = 0; i < numKeys; i++)
            {
                CanopyEventDetails_t eventDetails;

                SDDLControl control = sddl_class_lookup_control(
                        canopy->sddl,
                        keysArray[i]);
                if (!control)
                {
                    RedLog_WarnLog("canopy", "Control not found:", keysArray[i]);
                    continue;
                }
                void * oldVal = sddl_control_extra(control);
                free(oldVal);

                _CanopyPropertyValue *pVal = malloc(sizeof(_CanopyPropertyValue));
                pVal->datatype = sddl_control_datatype(control);

                if (RedJsonObject_IsValueNumber(controlObj, keysArray[i]))
                {
                    pVal->val.val_int8 = (int8_t)RedJsonObject_GetNumber(controlObj, keysArray[i]);
                }
                /* TODO: handle other datatypes */
                sddl_control_set_extra(control, pVal);

                /* Call event callback */
                eventDetails.ctx = canopy;
                eventDetails.eventType = CANOPY_EVENT_CONTROL_TRIGGER;
                eventDetails.userData = canopy->cbExtra;
                eventDetails.eventControlName = keysArray[i];
                eventDetails.value = *pVal;
                printf("Calling event callback CANOPY_EVENT_CONTROL_TRIGGER %s\n", keysArray[i]);
                canopy->cb(canopy, &eventDetails);
            }
        }
    }
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
        {
            CanopyEventDetails_t eventDetails;
            fprintf(stderr, "ws_callback: LWS_CALLBACK_CLIENT_ESTABLISHED\n");
            libwebsocket_callback_on_writable(this, wsi);

            /* Call event callback */
            eventDetails.ctx = canopy;
            eventDetails.eventType = CANOPY_EVENT_CONNECTION_ESTABLISHED;
            eventDetails.userData = canopy->cbExtra;
            canopy->cb(canopy, &eventDetails);

            break;
        }
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
            /* TODO: this next line seems dangerous! */
            ((char *)in)[len] = '\0';
            fprintf(stderr, "rx %d '%s'\n", (int)len, (char *)in);
            _process_ws_payload(canopy, in);
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
            canopy->cloudPort, 
            CANOPY_WS_USE_SSL, 
            "/echo",
            canopy->cloudHost,
            "localhost", /*origin?*/
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
