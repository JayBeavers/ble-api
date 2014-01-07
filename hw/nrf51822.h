#ifndef __NRF51822_H__
#define __NRF51822_H__

#include "mbed.h"
#include "blecommon.h"
#include "bleradio.h"
#include "GattService.h"

/**************************************************************************/
/*!
    \brief
    Driver for the nRF51822 in connectivity mode using custom serialization
    firmware.
*/
/**************************************************************************/
class nRF51822 : public BLERadio
{
    public:
        nRF51822();
        virtual ~nRF51822(void);

        /* Functions that mus be implemented from BLERadio */
        virtual ble_error_t setAdvertising(GapAdvertisingParams &, GapAdvertisingData &, GapAdvertisingData &);
        virtual ble_error_t addService(GattService &);
        virtual ble_error_t readCharacteristic(GattService &, GattCharacteristic &, uint8_t[], uint16_t);
        virtual ble_error_t writeCharacteristic(GattService &, GattCharacteristic &, uint8_t[], uint16_t);
        virtual ble_error_t start(void);
        virtual ble_error_t stop(void);
        virtual ble_error_t reset(void);
        
    private:
        RawSerial uart;

        /* nRF51 Functions */
        void uartCallback(void);
};

#endif
