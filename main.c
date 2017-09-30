/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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
 
// https://developer.mbed.org/teams/Bluetooth-Low-Energy/code/BLE_iBeacon/
// Company identifiers:
// https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
#include "mbed.h"
#include "ble/services/iBeacon.h"
 
/** User interface I/O **/

// instantiate USB Serial
Serial serial(USBTX, USBRX);

// Status LED
DigitalOut statusLed(LED1, 0);

// Timer for blinking the statusLed
Ticker ticker;

/** Bluetooth Peripheral Properties **/

// Bluetooth Peripheral Singleton

// Beacon UUID - use the same UUID for a group of Beacons
static const uint8_t UUID[] = {0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4,
                                0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61};

// Major Number and Minor number identify the Beacon
static const uint16_t MAJOR_NUMBER = 1122;
static const uint16_t MINOR_NUMBER = 3344;

// radio transmission power 
// 0xC8 = 200, 2's compliment is 256-200 = (-56dB)
static const uint16_t TRANSMISSION_POWER = 0xC8; 

/** Functions **/

/**
 * visually signal that program has not crashed
 */
void blinkHeartbeat(void);

/**
 * Callback triggered when the ble initialization process has finished
 *
 * @param[in] params Information about the initialized Peripheral
 */
void onBluetoothInitialized(BLE::InitializationCompleteCallbackContext *params);

/**
 * Main program and loop
 */
int main(void) {
    serial.baud(9600);
    serial.printf("Starting iBeacon\r\n");

    ticker.attach(blinkHeartbeat, 1); // Blink LED every 1 seconds 
    
    // initialized Bluetooth Radio
    BLE &ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    ble.init(onBluetoothInitialized);
    
    // wait for Bluetooth Radio to be initialized
    while (ble.hasInitialized()  == false);

    while (1) {
        // put radio to sleep in between broadcasts
        ble.waitForEvent(); 
    }

}

void blinkHeartbeat(void) {
    statusLed = !statusLed; /* Do blinky on LED1 to indicate system aliveness. */
}
    
void onBluetoothInitialized(BLE::InitializationCompleteCallbackContext *params) {
    BLE &ble          = params->ble;
    ble_error_t error = params->error;
 
    if (error != BLE_ERROR_NONE) {
        return;
    }

    // create beacon with these properties
    iBeacon *ibeacon = new iBeacon(ble, UUID, MAJOR_NUMBER, MINOR_NUMBER, TRANSMISSION_POWER);
 
    // begin broadcast
    ble.gap().setAdvertisingInterval(1000); // 1000ms
    ble.gap().startAdvertising();
}