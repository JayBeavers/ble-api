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

#include "DiscoveredCharacteristic.h"
#include "GattClient.h"

DiscoveredCharacteristic::ReadCallback_t DiscoveredCharacteristic::onDataReadCallback;

/**
 * Initiate (or continue) a read for the value attribute, optionally at a
 * given offset. If the Characteristic or Descriptor to be read is longer
 * than ATT_MTU - 1, this function must be called multiple times with
 * appropriate offset to read the complete value.
 *
 * @return BLE_ERROR_NONE if a read has been initiated, else
 *         BLE_ERROR_INVALID_STATE if some internal state about the connection is invalid, or
 *         BLE_STACK_BUSY if some client procedure already in progress, or
 *         BLE_ERROR_OPERATION_NOT_PERMITTED due to the characteristic's properties.
 */
ble_error_t
DiscoveredCharacteristic::read(uint16_t offset) const
{
    if (!props.read()) {
        return BLE_ERROR_OPERATION_NOT_PERMITTED;
    }

    if (!gattc) {
        return BLE_ERROR_INVALID_STATE;
    }

    return gattc->read(connHandle, valueHandle, offset);
}

/**
 * Perform a write without response procedure.
 *
 * @param  length
 *           The amount of data being written.
 * @param  value
 *           The bytes being written.
 *
 * @note   It is important to note that a write without response will
 *         <b>consume an application buffer</b>, and will therefore
 *         generate a onDataSent() callback when the packet has been
 *         transmitted. The application should ensure that the buffer is
 *         valid until the callback.
 *
 * @retval BLE_ERROR_NONE Successfully started the Write procedure, else
 *         BLE_ERROR_INVALID_STATE if some internal state about the connection is invalid, or
 *         BLE_STACK_BUSY if some client procedure already in progress, or
 *         BLE_ERROR_NO_MEM if there are no available buffers left to process the request, or
 *         BLE_ERROR_OPERATION_NOT_PERMITTED due to the characteristic's properties.
 */
ble_error_t
DiscoveredCharacteristic::writeWoResponse(uint16_t length, const uint8_t *value) const
{
    if (!props.writeWoResp()) {
        return BLE_ERROR_OPERATION_NOT_PERMITTED;
    }

    return gattc->write(GattClient::OP_WRITE_CMD, connHandle, length, value);
}