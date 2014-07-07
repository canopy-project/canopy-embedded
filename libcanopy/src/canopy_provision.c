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

typedef struct CanopyProvisionResults_t
{
    char *uuid;
} CanopyProvisionResults_t;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    RedStringList response = (RedStringList)userdata;
    RedStringList_AppendChars(response, ptr);
    return size*nmemb;
}

CanopyProvisionResults canopy_provision(CanopyContext canopy, const char *cloudUsername, const char *cloudPassword)
{
    CURL *curl = NULL;
    char *url = NULL;
    char *usernamePassword = NULL;
    bool success = false;
    RedStringList responseSL;
    char * responseBody;
    CanopyProvisionResults results;
    RedJsonObject responseJson;

    responseSL = RedStringList_New();

    results = calloc(1, sizeof(CanopyProvisionResults));
    if (!results)
    {
        RedLog_Error("OOM allocating CanopyProvisionResults");
        goto cleanup;
    }

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
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, responseSL);

    curl_easy_perform(curl);

    /* TODO: check server response */
    responseBody = RedStringList_ToNewChars(responseSL);
    RedStringList_Free(responseSL);
    if (!responseBody)
    {
        RedLog_Error("OOM collapsing response into string");
        goto cleanup;
    }

    /* Parse response body */
    responseJson = RedJson_Parse(responseBody);
    if (!responseJson)
    {
        RedLog_Error("Error parsing provisioning response JSON");
        goto cleanup;
    }

    if (!RedJsonObject_IsValueString(responseJson, "device_id"))
    {
        RedLog_Error("Expected string \"device_id\" in provisioning response:\n%s", responseBody);
        goto cleanup;
    }
    results->uuid = RedJsonObject_GetString(responseJson, "device_id");

    success = true;

cleanup:
    free(usernamePassword);
    free(url);
    if (curl);
        curl_easy_cleanup(curl);

    if (!success)
    {
        free(results);
        return NULL;
    }
    return results;
}

const char * canopy_provision_get_uuid(CanopyProvisionResults results)
{
    return results->uuid;
}

void canopy_free_provision_results(CanopyProvisionResults results)
{
    if (results)
    {
        free(results->uuid);
        free(results);
    }
}

