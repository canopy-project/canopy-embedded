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

// Stub implementation of Canopy HTTP utility library.
// This implementation is used when "curl" isn't available and HTTP is not needed.

#include "http/st_http.h"

CanopyResultEnum st_http_post(
        CanopyContext ctx, 
        const char *url, 
        const char *payload, 
        CanopyPromise *outPromise)
{
    return CANOPY_ERROR_PROTOCOL_NOT_SUPPORTED;
}


