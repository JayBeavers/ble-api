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

#ifndef __UUID_H__
#define __UUID_H__

#include "blecommon.h"

const unsigned   LENGTH_OF_LONG_UUID = 16;
typedef uint16_t ShortUUIDBytes_t;
typedef uint8_t  LongUUIDBytes_t[LENGTH_OF_LONG_UUID];

class UUID {
public:
    enum UUID_Type_t {
        UUID_TYPE_SHORT = 0,    // Short BLE UUID
        UUID_TYPE_LONG  = 1     // Full 128-bit UUID
    };

public:
    UUID(const LongUUIDBytes_t);
    UUID(ShortUUIDBytes_t);

public:
    UUID_Type_t       shortOrLong(void)  const {return type;     }
    const uint8_t    *getBaseUUID(void)  const {
        if (type == UUID_TYPE_SHORT) {
            return (const uint8_t*)&shortUUID;
        } else {
            return baseUUID;
        }
    }
    ShortUUIDBytes_t  getShortUUID(void) const {return shortUUID;}
    uint8_t           getLen(void)       const {
        return ((type == UUID_TYPE_SHORT) ? sizeof(ShortUUIDBytes_t) : LENGTH_OF_LONG_UUID);
    }

    bool operator== (const UUID&) const;

private:
    UUID_Type_t      type;      // UUID_TYPE_SHORT or UUID_TYPE_LONG
    LongUUIDBytes_t  baseUUID;  /* the base of the long UUID (if
                            * used). Note: bytes 12 and 13 (counting from LSB)
                            * are zeroed out to allow comparison with other long
                            * UUIDs which differ only in the 16-bit relative
                            * part.*/
    ShortUUIDBytes_t shortUUID; // 16 bit uuid (byte 2-3 using with base)
};

#endif // ifndef __UUID_H__