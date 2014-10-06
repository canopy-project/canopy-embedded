/*
 * Copyright 2014 SimpleThings, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <canopy.h>

int main(void) 
{
    canopy_post_sample(
        CANOPY_CLOUD_SERVER, "dev02.canopy.link",
        CANOPY_DEVICE_UUID, "9dfe2a00-efe2-45f9-a84c-8afc69caf4e7",
        CANOPY_PROPERTY_NAME, "cpu",
        CANOPY_VALUE_FLOAT32, 0.22f
    );
    return 0;
}
