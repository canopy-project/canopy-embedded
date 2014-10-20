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

#ifndef ST_HTTP_INCLUDED
#define ST_HTTP_INCLUDED

// HTTP utility library for Canopy.

#include <canopy.h>

// Send an HTTP POST request.
// <url> is the URL to POST to.
// <payload> is the request body.
// <outPromise>, if non-NULL, gets set to the address of a newly-allocated
// promise object that can be used to wait for completion of the request.
CanopyResultEnum st_http_post(
        CanopyContext ctx, 
        const char *url, 
        const char *payload, 
        CanopyPromise *outPromise);

#endif // ST_HTTP_INCLUDED
