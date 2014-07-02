/*
 * Copyright 2014 - Greg Prisament
 */
#include "canopy.h"
#include "canopy_internal.h"
#include "red_hash.h"
#include "red_json.h"
#include "red_log.h"
#include "red_string.h"
#include <unistd.h>
#include <assert.h>
#include <curl/curl.h>

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    printf("RECIEVED: %s", (char *)ptr);
    return size*nmemb;
}

bool canopy_provision(CanopyContext canopy, const char *cloudUsername, const char *cloudPassword)
{
    CURL *curl = NULL;
    char *url = NULL;
    char *usernamePassword = NULL;
    bool rval = false;

    curl = curl_easy_init();
    if (!curl)
    {
        RedLog_Error("Failure initializing curl");
        goto cleanup;
    }

    url = RedString_PrintfToNewChars("%s://%s:%d/create_device",
            canopy_get_web_protocol(canopy),
            canopy_get_cloud_host(canopy),
            canopy_get_cloud_port(canopy));
    if (!url)
    {
        RedLog_Error("OOM constructing create_account URL string");
        goto cleanup;
    }

    usernamePassword = RedString_PrintfToNewChars("%s:%s",
            cloudUsername, 
            cloudPassword);
    if (!usernamePassword)
    {
        RedLog_Error("OOM constructing BASIC AUTH string");
        goto cleanup;
    }

    /* POST to "/provision" endpoint */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERPWD, usernamePassword);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

    curl_easy_perform(curl);
    rval = true;

cleanup:
    free(usernamePassword);
    free(url);
    if (curl);
        curl_easy_cleanup(curl);

    return rval;
}
