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

#ifndef __GAP_EVENTS_H__
#define __GAP_EVENTS_H__

#include "blecommon.h"
#include "mbed.h"

/**************************************************************************/
/*!
    \brief
    The base class used to abstract away the callback events that can be
    triggered with the GAP.
*/
/**************************************************************************/
class GapEvents
{
public:
    /******************************************************************/
    /*!
        \brief
        Identifies GAP events generated by the radio HW when an event
        callback occurs
    */
    /******************************************************************/
    typedef enum gapEvent_e {
        GAP_EVENT_TIMEOUT      = 1, /**< Advertising timed out before a connection was established */
        GAP_EVENT_CONNECTED    = 2, /**< A connection was established with a central device */
        GAP_EVENT_DISCONNECTED = 3  /**< A connection was closed or lost with a central device */
    } gapEvent_t;
};

#endif // ifndef __GAP_EVENTS_H__