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

#ifndef __GAP_H__
#define __GAP_H__

#include "GapAdvertisingData.h"
#include "GapAdvertisingParams.h"
#include "GapEvents.h"
#include "CallChain.h"

using namespace mbed;

class Gap {
public:
    typedef enum addr_type_e {
        ADDR_TYPE_PUBLIC = 0,
        ADDR_TYPE_RANDOM_STATIC,
        ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE,
        ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE
    } addr_type_t;

    static const unsigned ADDR_LEN = 6;
    typedef uint8_t address_t[ADDR_LEN]; /* 48-bit address, LSB format. */

    /**
     * Enumeration for disconnection reasons. The values for these reasons are
     * derived from Nordic's implementation; but the reasons are meant to be
     * independent of the transport. If you are returned a reason which is not
     * covered by this enumeration, then please refer to the underlying
     * transport library.
     */
    enum DisconnectionReason_t {
        REMOTE_USER_TERMINATED_CONNECTION = 0x13,
        LOCAL_HOST_TERMINATED_CONNECTION  = 0x16,
        CONN_INTERVAL_UNACCEPTABLE        = 0x3B,
    };

    /* Describes the current state of the device (more than one bit can be set) */
    typedef struct GapState_s {
        unsigned advertising : 1; /**< peripheral is currently advertising */
        unsigned connected   : 1; /**< peripheral is connected to a central */
    } GapState_t;

    typedef uint16_t Handle_t;

    typedef struct {
        uint16_t minConnectionInterval;      /**< Minimum Connection Interval in 1.25 ms units, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t maxConnectionInterval;      /**< Maximum Connection Interval in 1.25 ms units, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t slaveLatency;               /**< Slave Latency in number of connection events, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t connectionSupervisionTimeout; /**< Connection Supervision Timeout in 10 ms units, see @ref BLE_GAP_CP_LIMITS.*/
    } ConnectionParams_t;

    /**
     * @brief Defines possible security status/states.
     *
     * @details Defines possible security status/states of a link when requested by getLinkSecurity().
     */
    enum LinkSecurityStatus_t {
        NOT_ENCRYPTED,          /**< The link is not secured. */
        ENCRYPTION_IN_PROGRESS, /**< Link security is being established.*/
        ENCRYPTED               /**< The link is secure.*/
    };

    static const uint16_t UNIT_1_25_MS  = 1250; /**< Number of microseconds in 1.25 milliseconds. */
    static const uint16_t UNIT_0_625_MS = 650;  /**< Number of microseconds in 0.625 milliseconds. */
    static uint16_t MSEC_TO_GAP_DURATION_UNITS(uint32_t durationInMillis) {
        return (durationInMillis * 1000) / UNIT_1_25_MS;
    }
    static uint16_t MSEC_TO_ADVERTISEMENT_DURATION_UNITS(uint32_t durationInMillis) {
        return (durationInMillis * 1000) / UNIT_0_625_MS;
    }
    static uint16_t GAP_DURATION_UNITS_TO_MS(uint16_t gapUnits) {
        return (gapUnits * UNIT_0_625_MS) / 1000;
    }

    typedef void (*EventCallback_t)(void);
    typedef void (*ConnectionEventCallback_t)(Handle_t,
                                              addr_type_t peerAddrType, const address_t peerAddr,
                                              addr_type_t ownAddrType,  const address_t ownAddr,
                                              const ConnectionParams_t *);
    typedef void (*HandleSpecificEvent_t)(Handle_t handle);
    typedef void (*DisconnectionEventCallback_t)(Handle_t, DisconnectionReason_t);
    typedef void (*RadioNotificationEventCallback_t) (bool radio_active); /* gets passed true for ACTIVE; false for INACTIVE. */

    friend class BLEDevice;
private:
    /* These functions must be defined in the sub-class */
    virtual ble_error_t setAddress(addr_type_t type,   const address_t address)                    = 0;
    virtual ble_error_t getAddress(addr_type_t *typeP, address_t address)                          = 0;
    virtual ble_error_t setAdvertisingData(const GapAdvertisingData &, const GapAdvertisingData &) = 0;
    virtual ble_error_t startAdvertising(const GapAdvertisingParams &)                             = 0;
    virtual ble_error_t stopAdvertising(void)                                                      = 0;
    virtual uint16_t    getMinAdvertisingInterval(void) const                                      = 0;
    virtual uint16_t    getMinNonConnectableAdvertisingInterval(void) const                        = 0;
    virtual uint16_t    getMaxAdvertisingInterval(void) const                                      = 0;
    virtual ble_error_t disconnect(DisconnectionReason_t reason)                                   = 0;
    virtual ble_error_t getPreferredConnectionParams(ConnectionParams_t *params)                   = 0;
    virtual ble_error_t setPreferredConnectionParams(const ConnectionParams_t *params)             = 0;
    virtual ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params)  = 0;

    virtual ble_error_t deleteAllBondedDevices(void)                                                      = 0;
    virtual ble_error_t getLinkSecurity(Handle_t connectionHandle, LinkSecurityStatus_t *securityStatusP) = 0;

    virtual ble_error_t setDeviceName(const uint8_t *deviceName)              = 0;
    virtual ble_error_t getDeviceName(uint8_t *deviceName, unsigned *lengthP) = 0;
    virtual ble_error_t setAppearance(uint16_t appearance)                    = 0;
    virtual ble_error_t getAppearance(uint16_t *appearanceP)                  = 0;

protected:
    /* Event callback handlers */
    void setOnTimeout(EventCallback_t callback) {onTimeout = callback;}
    void setOnConnection(ConnectionEventCallback_t callback) {onConnection = callback;}

    /**
     * Set the application callback for disconnection events.
     * @param callback
     *        Pointer to the unique callback.
     */
    void setOnDisconnection(DisconnectionEventCallback_t callback) {onDisconnection = callback;}

    /**
     * Set the application callback for radio-notification events.
     * @param callback
     *          Handler to be executed in resonse to a radio notification event.
     */
    virtual void setOnRadioNotification(RadioNotificationEventCallback_t callback) {onRadioNotification = callback;}

    /**
     * To indicate that security procedure for link has started.
     */
    virtual void setOnSecuritySetupStarted(HandleSpecificEvent_t callback) {onSecuritySetupStarted = callback;}

    /**
     * To indicate that security procedure for link has completed.
     */
    virtual void setOnSecuritySetupCompleted(HandleSpecificEvent_t callback) {onSecuritySetupCompleted = callback;}

    /**
     * To indicate that link with the peer is secured. For bonded devices,
     * subsequent reconnections with bonded peer will result only in this callback
     * when the link is secured and setup procedures will not occur unless the
     * bonding information is either lost or deleted on either or both sides.
     */
    virtual void setOnLinkSecured(HandleSpecificEvent_t callback) {onLinkSecured = callback;}

    /**
     * To indicate that device context is stored persistently.
     */
    virtual void setOnSecurityContextStored(HandleSpecificEvent_t callback) {onSecurityContextStored = callback;}

    /**
     * Append to a chain of callbacks to be invoked upon disconnection; these
     * callbacks receive no context and are therefore different from the
     * onDisconnection callback.
     * @param callback
     *        function pointer to be invoked upon disconnection; receives no context.
     *
     * @note the disconnection CallChain should have been merged with
     *     onDisconnctionCallback; but this was not possible because
     *     FunctionPointer (which is a building block for CallChain) doesn't
     *     accept variadic templates.
     */
    template<typename T>
    void addToDisconnectionCallChain(T *tptr, void (T::*mptr)(void)) {disconnectionCallChain.add(tptr, mptr);}

private:
    GapState_t getState(void) const {
        return state;
    }

protected:
    Gap() :
        state(),
        onTimeout(NULL),
        onConnection(NULL),
        onDisconnection(NULL),
        onRadioNotification(),
        onSecuritySetupStarted(),
        onSecuritySetupCompleted(),
        onLinkSecured(),
        onSecurityContextStored(),
        disconnectionCallChain() {
        /* empty */
    }

public:
    void processConnectionEvent(Handle_t handle, addr_type_t peerAddrType, const address_t peerAddr, addr_type_t ownAddrType, const address_t ownAddr, const ConnectionParams_t *params) {
        state.connected = 1;
        if (onConnection) {
            onConnection(handle, peerAddrType, peerAddr, ownAddrType, ownAddr, params);
        }
    }

    void processDisconnectionEvent(Handle_t handle, DisconnectionReason_t reason) {
        state.connected = 0;
        if (onDisconnection) {
            onDisconnection(handle, reason);
        }
        disconnectionCallChain.call();
    }

    void processSecuritySetupStartedEvent(Handle_t handle) {
        if (onSecuritySetupStarted) {
            onSecuritySetupStarted(handle);
        }
    }

    void processSecuritySetupCompletedEvent(Handle_t handle) {
        if (onSecuritySetupCompleted) {
            onSecuritySetupCompleted(handle);
        }
    }

    void processLinkSecuredEvent(Handle_t handle) {
        if (onLinkSecured) {
            onLinkSecured(handle);
        }
    }

    void processSecurityContextStoredEvent(Handle_t handle) {
        if (onSecurityContextStored) {
            onSecurityContextStored(handle);
        }
    }

    void processEvent(GapEvents::gapEvent_e type) {
        switch (type) {
            case GapEvents::GAP_EVENT_TIMEOUT:
                state.advertising = 0;
                if (onTimeout) {
                    onTimeout();
                }
                break;
            default:
                break;
        }
    }

protected:
    GapState_t                   state;

protected:
    EventCallback_t              onTimeout;
    ConnectionEventCallback_t    onConnection;
    DisconnectionEventCallback_t onDisconnection;
    RadioNotificationEventCallback_t onRadioNotification;
    HandleSpecificEvent_t        onSecuritySetupStarted;
    HandleSpecificEvent_t        onSecuritySetupCompleted;
    HandleSpecificEvent_t        onLinkSecured;
    HandleSpecificEvent_t        onSecurityContextStored;
    CallChain                    disconnectionCallChain;

private:
    /* disallow copy and assignment */
    Gap(const Gap &);
    Gap& operator=(const Gap &);
};

#endif // ifndef __GAP_H__