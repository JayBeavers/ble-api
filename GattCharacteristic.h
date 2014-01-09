/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
  

#ifndef __GATT_CHARACTERISTIC_H__
#define __GATT_CHARACTERISTIC_H__

#include "blecommon.h"
#include "UUID.h"

class GattCharacteristic
{
private:

public:
    GattCharacteristic(uint16_t uuid=0, uint16_t minLen=1, uint16_t maxLen=1, uint8_t properties=0);
    virtual ~GattCharacteristic(void);

    uint16_t uuid;              /* Characteristic UUID */
    uint16_t lenMin;            /* Minimum length of the value */
    uint16_t lenMax;            /* Maximum length of the value */
    uint8_t  handle;
    uint8_t  properties;
};

#endif
