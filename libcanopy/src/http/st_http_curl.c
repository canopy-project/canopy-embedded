// Copyright 2014 SimpleThings, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// HTTP utility library for Canopy.

#include "http/st_http.h"
#include "red_string.h"
#include <curl/curl.h>

// Handler for CURL write callback.  Concatenates received bytes into a
// RedStringList object, for easy conversion to a string when the whole
// response has been received.
static size_t _curl_write_handler(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    RedStringList response = (RedStringList)userdata;
    RedStringList_AppendChars(response, ptr);
    return size*nmemb;
}

// Implemented using libcurl's "easy" API.
CanopyResultEnum st_http_post(
        CanopyCtx ctx, 
        const char *url, 
        const char *payload, 
        CanopyPromise *outPromise)
{
    // TODO: fix cleanup & error handling
    CURL *curl = NULL;
    RedStringList response_sl;
    char *response_body;
    //RedJsonObject response_json;

    printf("Sending payload to %s:\n%s\n\n", url, payload);

    response_sl = RedStringList_New();

    /*
    results = calloc(1, sizeof(_CanopyHTTPResults));
    if (!results)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }*/

    curl = curl_easy_init();
    if (!curl)
    {
        goto cleanup;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_handler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_sl);

    curl_easy_perform(curl);

    // TODO: check server response
    response_body = RedStringList_ToNewChars(response_sl);
    if (!response_body)
    {
        return CANOPY_ERROR_OUT_OF_MEMORY;
    }
    printf("Response: %s\n", response_body);

    // Parse response body
    /*response_json = RedJson_Parse(response_body);
    if (!response_json)
    {
        return CANOPY_ERROR_UNKNOWN;
    }
    */
    return CANOPY_SUCCESS;

cleanup:
    RedStringList_Free(response_sl);
    return CANOPY_ERROR_UNKNOWN;
}

