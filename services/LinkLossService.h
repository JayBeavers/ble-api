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

#ifndef __BLE_LINK_LOSS_SERVICE_H__
#define __BLE_LINK_LOSS_SERVICE_H__

#include "Gap.h"

/**
* @class LinkLossService
* @brief This service defines behavior when a link is lost between two devices. <br>
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.link_loss.xml <br>
* Alertness Level Char: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.alert_level.xml <br>
*/
class LinkLossService {
public:
    enum AlertLevel_t {
        NO_ALERT   = 0,
        MILD_ALERT = 1,
        HIGH_ALERT = 2
    };

    typedef void (* callback_t)(AlertLevel_t level);

    /**
     * @param[ref] ble
     *                 BLEDevice object for the underlying controller.
     */
    LinkLossService(BLEDevice &bleIn, callback_t callbackIn, AlertLevel_t levelIn = NO_ALERT) :
        ble(bleIn),
        alertLevel(levelIn),
        callback(callbackIn),
        alertLevelChar(GattCharacteristic::UUID_ALERT_LEVEL_CHAR, reinterpret_cast<uint8_t *>(&alertLevel), sizeof(uint8_t), sizeof(uint8_t),
                       GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE) {
        static bool serviceAdded = false; /* We should only ever add one LinkLoss service. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = {&alertLevelChar};
        GattService         linkLossService(GattService::UUID_LINK_LOSS_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(linkLossService);
        serviceAdded = true;

        ble.addToDisconnectionCallChain(this, &LinkLossService::onDisconnectionFilter);
        ble.onDataWritten(this, &LinkLossService::onDataWritten);
    }

    /**
     * Update the callback.
     */
    void setCallback(callback_t newCallback) {
        callback = newCallback;
    }

    /**
     * Update Alertness Level.
     */
    void setAlertLevel(AlertLevel_t newLevel) {
        alertLevel = newLevel;
    }

private:
    /**
     * This callback allows receiving updates to the AlertLevel Characteristic.
     *
     * @param[in] params
     *     Information about the characterisitc being updated.
     */
    virtual void onDataWritten(const GattCharacteristicWriteCBParams *params) {
        if (params->charHandle == alertLevelChar.getValueHandle()) {
            alertLevel = *reinterpret_cast<const AlertLevel_t *>(params->data);
        }
    }

    void onDisconnectionFilter(void) {
        if (alertLevel != NO_ALERT) {
            callback(alertLevel);
        }
    }

private:
    BLEDevice          &ble;
    AlertLevel_t        alertLevel;
    callback_t          callback;
    GattCharacteristic  alertLevelChar;
};

#endif /* __BLE_LINK_LOSS_SERVICE_H__ */