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
#include "GapScanningParams.h"
#include "GapEvents.h"
#include "CallChain.h"
#include "FunctionPointerWithContext.h"

using namespace mbed;

/* Forward declarations for classes which will only be used for pointers or references in the following. */
class GapAdvertisingParams;
class GapScanningParams;
class GapAdvertisingData;

class Gap {
public:
    enum AddressType_t {
        ADDR_TYPE_PUBLIC = 0,
        ADDR_TYPE_RANDOM_STATIC,
        ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE,
        ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE
    };
    typedef enum AddressType_t addr_type_t; /* @Note: deprecated. Use AddressType_t instead. */

    static const unsigned ADDR_LEN = 6;
    typedef uint8_t Address_t[ADDR_LEN]; /* 48-bit address, LSB format. */
    typedef Address_t address_t;         /* @Note: deprecated. Use Address_t instead. */

    enum AdvertisementType_t {
        ADV_IND           = 0x00,   /**< Connectable undirected. */
        ADV_DIRECT_IND    = 0x01,   /**< Connectable directed. */
        ADV_SCAN_IND      = 0x02,   /**< Scannable undirected. */
        ADV_NONCONN_IND   = 0x03,   /**< Non connectable undirected. */
    };

    /**
     * Enumeration for disconnection reasons. The values for these reasons are
     * derived from Nordic's implementation; but the reasons are meant to be
     * independent of the transport. If you are returned a reason which is not
     * covered by this enumeration, then please refer to the underlying
     * transport library.
     */
    enum DisconnectionReason_t {
        CONNECTION_TIMEOUT                          = 0x08,
        REMOTE_USER_TERMINATED_CONNECTION           = 0x13,
        REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES = 0x14,  /**< Remote Device Terminated Connection due to low resources.*/
        REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF     = 0x15,  /**< Remote Device Terminated Connection due to power off. */
        LOCAL_HOST_TERMINATED_CONNECTION            = 0x16,
        CONN_INTERVAL_UNACCEPTABLE                  = 0x3B,
    };

    /* Describes the current state of the device (more than one bit can be set) */
    struct GapState_t {
        unsigned advertising : 1; /**< peripheral is currently advertising */
        unsigned connected   : 1; /**< peripheral is connected to a central */
    };

    typedef uint16_t Handle_t;

    typedef struct {
        uint16_t minConnectionInterval;      /**< Minimum Connection Interval in 1.25 ms units, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t maxConnectionInterval;      /**< Maximum Connection Interval in 1.25 ms units, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t slaveLatency;               /**< Slave Latency in number of connection events, see @ref BLE_GAP_CP_LIMITS.*/
        uint16_t connectionSupervisionTimeout; /**< Connection Supervision Timeout in 10 ms units, see @ref BLE_GAP_CP_LIMITS.*/
    } ConnectionParams_t;

    enum Role_t {
        PERIPHERAL  = 0x1, /**< Peripheral Role. */
        CENTRAL     = 0x2, /**< Central Role.    */
    };

    struct AdvertisementCallbackParams_t {
        Address_t            peerAddr;
        int8_t               rssi;
        bool                 isScanResponse;
        AdvertisementType_t  type;
        uint8_t              advertisingDataLen;
        const uint8_t       *advertisingData;
    };
    typedef FunctionPointerWithContext<const AdvertisementCallbackParams_t *> AdvertisementReportCallback_t;

    struct ConnectionCallbackParams_t {
        Handle_t      handle;
        Role_t        role;
        AddressType_t peerAddrType;
        Address_t     peerAddr;
        AddressType_t ownAddrType;
        Address_t     ownAddr;
        const ConnectionParams_t *connectionParams;

        ConnectionCallbackParams_t(Handle_t       handleIn,
                                   Role_t         roleIn,
                                   AddressType_t  peerAddrTypeIn,
                                   const uint8_t *peerAddrIn,
                                   AddressType_t  ownAddrTypeIn,
                                   const uint8_t *ownAddrIn,
                                   const ConnectionParams_t *connectionParamsIn) :
            handle(handleIn),
            role(roleIn),
            peerAddrType(peerAddrTypeIn),
            peerAddr(),
            ownAddrType(ownAddrTypeIn),
            ownAddr(),
            connectionParams(connectionParamsIn) {
            memcpy(peerAddr, peerAddrIn, ADDR_LEN);
            memcpy(ownAddr, ownAddrIn, ADDR_LEN);
        }
    };

    enum SecurityMode_t {
        SECURITY_MODE_NO_ACCESS,
        SECURITY_MODE_ENCRYPTION_OPEN_LINK, /**< require no protection, open link. */
        SECURITY_MODE_ENCRYPTION_NO_MITM,   /**< require encryption, but no MITM protection. */
        SECURITY_MODE_ENCRYPTION_WITH_MITM, /**< require encryption and MITM protection. */
        SECURITY_MODE_SIGNED_NO_MITM,       /**< require signing or encryption, but no MITM protection. */
        SECURITY_MODE_SIGNED_WITH_MITM,     /**< require signing or encryption, and MITM protection. */
    };

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

    enum SecurityIOCapabilities_t {
      IO_CAPS_DISPLAY_ONLY     = 0x00,   /**< Display Only. */
      IO_CAPS_DISPLAY_YESNO    = 0x01,   /**< Display and Yes/No entry. */
      IO_CAPS_KEYBOARD_ONLY    = 0x02,   /**< Keyboard Only. */
      IO_CAPS_NONE             = 0x03,   /**< No I/O capabilities. */
      IO_CAPS_KEYBOARD_DISPLAY = 0x04,   /**< Keyboard and Display. */
    };

    enum SecurityCompletionStatus_t {
        SEC_STATUS_SUCCESS              = 0x00,  /**< Procedure completed with success. */
        SEC_STATUS_TIMEOUT              = 0x01,  /**< Procedure timed out. */
        SEC_STATUS_PDU_INVALID          = 0x02,  /**< Invalid PDU received. */
        SEC_STATUS_PASSKEY_ENTRY_FAILED = 0x81,  /**< Passkey entry failed (user canceled or other). */
        SEC_STATUS_OOB_NOT_AVAILABLE    = 0x82,  /**< Out of Band Key not available. */
        SEC_STATUS_AUTH_REQ             = 0x83,  /**< Authentication requirements not met. */
        SEC_STATUS_CONFIRM_VALUE        = 0x84,  /**< Confirm value failed. */
        SEC_STATUS_PAIRING_NOT_SUPP     = 0x85,  /**< Pairing not supported.  */
        SEC_STATUS_ENC_KEY_SIZE         = 0x86,  /**< Encryption key size. */
        SEC_STATUS_SMP_CMD_UNSUPPORTED  = 0x87,  /**< Unsupported SMP command. */
        SEC_STATUS_UNSPECIFIED          = 0x88,  /**< Unspecified reason. */
        SEC_STATUS_REPEATED_ATTEMPTS    = 0x89,  /**< Too little time elapsed since last attempt. */
        SEC_STATUS_INVALID_PARAMS       = 0x8A,  /**< Invalid parameters. */
    };

    /**
     * Declaration of type containing a passkey to be used during pairing. This
     * is passed into initializeSecurity() to specify a pre-programmed passkey
     * for authentication instead of generating a random one.
     */
    static const unsigned PASSKEY_LEN = 6;
    typedef uint8_t Passkey_t[PASSKEY_LEN];         /**< 6-digit passkey in ASCII ('0'-'9' digits only). */

    static const uint16_t UNIT_1_25_MS  = 1250; /**< Number of microseconds in 1.25 milliseconds. */
    static const uint16_t UNIT_0_625_MS = 625;  /**< Number of microseconds in 0.625 milliseconds. */
    static uint16_t MSEC_TO_GAP_DURATION_UNITS(uint32_t durationInMillis) {
        return (durationInMillis * 1000) / UNIT_1_25_MS;
    }
    static uint16_t MSEC_TO_ADVERTISEMENT_DURATION_UNITS(uint32_t durationInMillis) {
        return (durationInMillis * 1000) / UNIT_0_625_MS;
    }
    static uint16_t ADVERTISEMENT_DURATION_UNITS_TO_MS(uint16_t gapUnits) {
        return (gapUnits * UNIT_0_625_MS) / 1000;
    }

    typedef void (*EventCallback_t)(void);
    typedef void (*ConnectionEventCallback_t)(const ConnectionCallbackParams_t *params);
    typedef void (*HandleSpecificEvent_t)(Handle_t handle);
    typedef void (*DisconnectionEventCallback_t)(Handle_t, DisconnectionReason_t);
    typedef void (*RadioNotificationEventCallback_t) (bool radio_active); /* gets passed true for ACTIVE; false for INACTIVE. */
    typedef void (*SecuritySetupInitiatedCallback_t)(Handle_t, bool allowBonding, bool requireMITM, SecurityIOCapabilities_t iocaps);
    typedef void (*SecuritySetupCompletedCallback_t)(Handle_t, SecurityCompletionStatus_t status);
    typedef void (*LinkSecuredCallback_t)(Handle_t handle, SecurityMode_t securityMode);
    typedef void (*PasskeyDisplayCallback_t)(Handle_t handle, const Passkey_t passkey);

public:
    /* These functions must be defined in the sub-class */
    virtual ble_error_t setAddress(AddressType_t type,  const Address_t address)                   = 0;
    virtual ble_error_t getAddress(AddressType_t *typeP, Address_t address)                        = 0;
    virtual ble_error_t stopAdvertising(void)                                                      = 0;
    virtual ble_error_t stopScan()                                                                 = 0;
    virtual uint16_t    getMinAdvertisingInterval(void) const                                      = 0;
    virtual uint16_t    getMinNonConnectableAdvertisingInterval(void) const                        = 0;
    virtual uint16_t    getMaxAdvertisingInterval(void) const                                      = 0;
    virtual ble_error_t connect(const Address_t           peerAddr,
                                Gap::AddressType_t        peerAddrType,
                                const ConnectionParams_t *connectionParams,
                                const GapScanningParams  *scanParams) = 0;
    virtual ble_error_t disconnect(DisconnectionReason_t reason)                                   = 0;
    virtual ble_error_t getPreferredConnectionParams(ConnectionParams_t *params)                   = 0;
    virtual ble_error_t setPreferredConnectionParams(const ConnectionParams_t *params)             = 0;
    virtual ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params)  = 0;

    virtual ble_error_t purgeAllBondingState(void)                                                        = 0;
    virtual ble_error_t getLinkSecurity(Handle_t connectionHandle, LinkSecurityStatus_t *securityStatusP) = 0;

    virtual ble_error_t setDeviceName(const uint8_t *deviceName)              = 0;
    virtual ble_error_t getDeviceName(uint8_t *deviceName, unsigned *lengthP) = 0;
    virtual ble_error_t setAppearance(uint16_t appearance)                    = 0;
    virtual ble_error_t getAppearance(uint16_t *appearanceP)                  = 0;

    virtual ble_error_t setTxPower(int8_t txPower)                            = 0;
    virtual void        getPermittedTxPowerValues(const int8_t **, size_t *)  = 0;

    ble_error_t startAdvertising(void) {
        return startAdvertising(_advParams);
    }

    ble_error_t setAdvertisingData(void) {
        if (needToSetAdvPayload) {
            needToSetAdvPayload = false;
            return setAdvertisingData(_advPayload, _scanResponse);
        }

        return BLE_ERROR_NONE;
    }

private:
    virtual ble_error_t setAdvertisingData(const GapAdvertisingData &, const GapAdvertisingData &) = 0;
    virtual ble_error_t startAdvertising(const GapAdvertisingParams &)                             = 0;

public:
    ble_error_t startScan(void (*callback)(const AdvertisementCallbackParams_t *params)) {
        ble_error_t err = BLE_ERROR_NONE;
        if (callback) {
            if ((err = startRadioScan(_scanningParams)) == BLE_ERROR_NONE) {
                onAdvertisementReport.attach(callback);
            }
        }

        return err;
    }

    template<typename T>
    ble_error_t startScan(T *object, void (T::*callbackMember)(const AdvertisementCallbackParams_t *params)) {
        ble_error_t err = BLE_ERROR_NONE;
        if (object && callbackMember) {
            if ((err = startRadioScan(_scanningParams)) == BLE_ERROR_NONE) {
                onAdvertisementReport.attach(object, callbackMember);
            }
        }

        return err;
    }

public:
    virtual ble_error_t startRadioScan(const GapScanningParams &scanningParams) = 0;

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
     *          Handler to be executed in response to a radio notification event.
     */
    virtual void setOnRadioNotification(RadioNotificationEventCallback_t callback) {onRadioNotification = callback;}

    /**
     * To indicate that security procedure for link has started.
     */
    virtual void setOnSecuritySetupInitiated(SecuritySetupInitiatedCallback_t callback) {onSecuritySetupInitiated = callback;}

    /**
     * To indicate that security procedure for link has completed.
     */
    virtual void setOnSecuritySetupCompleted(SecuritySetupCompletedCallback_t callback) {onSecuritySetupCompleted = callback;}

    /**
     * To indicate that link with the peer is secured. For bonded devices,
     * subsequent re-connections with bonded peer will result only in this callback
     * when the link is secured and setup procedures will not occur unless the
     * bonding information is either lost or deleted on either or both sides.
     */
    virtual void setOnLinkSecured(LinkSecuredCallback_t callback) {onLinkSecured = callback;}

    /**
     * To indicate that device context is stored persistently.
     */
    virtual void setOnSecurityContextStored(HandleSpecificEvent_t callback) {onSecurityContextStored = callback;}

    /**
     * To set the callback for when the passkey needs to be displayed on a peripheral with DISPLAY capability.
     */
    virtual void setOnPasskeyDisplay(PasskeyDisplayCallback_t callback) {onPasskeyDisplay = callback;}

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

    GapAdvertisingParams &advParams(void) {
        return _advParams;
    }
    const GapAdvertisingParams &advParams(void) const {
        return _advParams;
    }
    void setAdvParams(const GapAdvertisingParams &newParams) {
        _advParams = newParams;
    }

    GapAdvertisingData &advPayload(void) {
        needToSetAdvPayload = true;
        return _advPayload;
    }
    const GapAdvertisingData &advPayload(void) const {
        return _advPayload;
    }

    GapAdvertisingData &scanResponse(void) {
        needToSetAdvPayload = true;
        return _scanResponse;
    }
    const GapAdvertisingData &scanResponse(void) const {
        return _scanResponse;
    }

    GapScanningParams &scanningParams(void) {
        return _scanningParams;
    }
    const GapScanningParams &scanningParams(void) const {
        return _scanningParams;
    }

public:
    GapState_t getState(void) const {
        return state;
    }

protected:
    Gap() :
        _advParams(),
        _advPayload(),
        needToSetAdvPayload(true),
        _scanningParams(),
        _scanResponse(),
        state(),
        onTimeout(NULL),
        onConnection(NULL),
        onDisconnection(NULL),
        onRadioNotification(),
        onSecuritySetupInitiated(),
        onSecuritySetupCompleted(),
        onLinkSecured(),
        onSecurityContextStored(),
        onPasskeyDisplay(),
        onAdvertisementReport(),
        disconnectionCallChain() {
        _advPayload.clear();
        _scanResponse.clear();
    }

public:
    void processConnectionEvent(Handle_t                  handle,
                                Role_t                    role,
                                AddressType_t             peerAddrType,
                                const Address_t           peerAddr,
                                AddressType_t             ownAddrType,
                                const Address_t           ownAddr,
                                const ConnectionParams_t *connectionParams) {
        state.connected = 1;
        if (onConnection) {
            ConnectionCallbackParams_t callbackParams(handle, role, peerAddrType, peerAddr, ownAddrType, ownAddr, connectionParams);
            onConnection(&callbackParams);
        }
    }

    void processDisconnectionEvent(Handle_t handle, DisconnectionReason_t reason) {
        state.connected = 0;
        if (onDisconnection) {
            onDisconnection(handle, reason);
        }
        disconnectionCallChain.call();
    }

    void processSecuritySetupInitiatedEvent(Handle_t handle, bool allowBonding, bool requireMITM, SecurityIOCapabilities_t iocaps) {
        if (onSecuritySetupInitiated) {
            onSecuritySetupInitiated(handle, allowBonding, requireMITM, iocaps);
        }
    }

    void processSecuritySetupCompletedEvent(Handle_t handle, SecurityCompletionStatus_t status) {
        if (onSecuritySetupCompleted) {
            onSecuritySetupCompleted(handle, status);
        }
    }

    void processLinkSecuredEvent(Handle_t handle, SecurityMode_t securityMode) {
        if (onLinkSecured) {
            onLinkSecured(handle, securityMode);
        }
    }

    void processSecurityContextStoredEvent(Handle_t handle) {
        if (onSecurityContextStored) {
            onSecurityContextStored(handle);
        }
    }

    void processPasskeyDisplayEvent(Handle_t handle, const Passkey_t passkey) {
        if (onPasskeyDisplay) {
            onPasskeyDisplay(handle, passkey);
        }
    }

    void processAdvertisementReport(const Address_t      peerAddr,
                                    int8_t               rssi,
                                    bool                 isScanResponse,
                                    AdvertisementType_t  type,
                                    uint8_t              advertisingDataLen,
                                    const uint8_t       *advertisingData) {
        AdvertisementCallbackParams_t params;
        memcpy(params.peerAddr, peerAddr, ADDR_LEN);
        params.rssi               = rssi;
        params.isScanResponse     = isScanResponse;
        params.type               = type;
        params.advertisingDataLen = advertisingDataLen;
        params.advertisingData    = advertisingData;
        onAdvertisementReport.call(&params);
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
    GapAdvertisingParams             _advParams;
    GapAdvertisingData               _advPayload;
    /* Accumulation of AD structures in the advertisement payload should
     * eventually result in a call to the target's setAdvertisingData() before
     * the server begins advertising. This flag marks the status of the pending update.*/
    bool                             needToSetAdvPayload;

    GapScanningParams                _scanningParams;
    GapAdvertisingData               _scanResponse;

    GapState_t                       state;

protected:
    EventCallback_t                  onTimeout;
    ConnectionEventCallback_t        onConnection;
    DisconnectionEventCallback_t     onDisconnection;
    RadioNotificationEventCallback_t onRadioNotification;
    SecuritySetupInitiatedCallback_t onSecuritySetupInitiated;
    SecuritySetupCompletedCallback_t onSecuritySetupCompleted;
    LinkSecuredCallback_t            onLinkSecured;
    HandleSpecificEvent_t            onSecurityContextStored;
    PasskeyDisplayCallback_t         onPasskeyDisplay;
    AdvertisementReportCallback_t    onAdvertisementReport;
    CallChain                        disconnectionCallChain;

private:
    /* disallow copy and assignment */
    Gap(const Gap &);
    Gap& operator=(const Gap &);
};

#endif // ifndef __GAP_H__