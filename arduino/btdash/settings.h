/*
Copyright (c) 2013, ROSSER ALPHA LLC
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of ROSSER ALPHA LLC nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __BTDASH_EEPROM_h
#define __BTDASH_EEPROM_h

#include "Arduino.h"
#include "units.h"

// input mapping
static const uint8_t PIN_TACH = 52;
static const uint8_t PIN_FUEL = A0;

// By default, communicate at this bitrate. The listener client must match this value.
static const uint32_t SERIAL_BPS = 115200;

#define VERSION_STRING "BTDASH_0.0.1"

// output mapping
#define SERIAL_BT Serial

// fuel configuration
#define FUEL_NUM_SEGMENTS      5
#define FUEL_MAX_SAMPLES_SAVED 20

static const uint8_t FUEL_UPDATE_EVERY_MS = 250; // number of ms between fuel updates

// EEPROM addresses
#if (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__))
#define EE_SIZE = 4096;
#else
#define EE_SIZE = 512;
#endif

#define EE_STATUS_BUFFER    0
#define EE_NUM_BUFFERS      5
#define EE_DATA_MAX_SIZE    (EE_SIZE - EE_NUM_BUFFERS)
#define EE_DATA_BUFFERS     (EE_STATUS_BUFFER + EE_NUM_BUFFERS)
#define EE_READ(a) eeprom_read_byte((uint8_t*)(a))
#define EE_WRITE(a,v) {\
    byte b = EE_READ(a);\
    if (b != (v)) eeprom_write_byte((uint8_t*)(a), (v));}

class FuelData
{
public:
    // ohm values for each fuel segment, from empty to full
    uint8_t ohmSegments[FUEL_NUM_SEGMENTS];

    // liters remaining for each fuel segment, from empty to full
    uint8_t litersSegments[FUEL_NUM_SEGMENTS];

    uint16_t nSamples;
    uint8_t sampleData[FUEL_MAX_SAMPLES_SAVED];
};

// packed version of Settings + UserData
typedef struct EE_Data
{
    // 2 bits for each Units saved
    // Packed like this:
    // Units distanceUnits;
    // Units pressureUnits;
    // Units temperatureUnits;
    // Units volumeUnits;
    uint8_t units;
    uint8_t fuelOhmScale;
    uint8_t tachDutyCycle;
    FuelData fuel;
    uint32_t distanceTraveled;
} EE_Data;

// user-configurable settings
class Settings
{
public:
    Settings()
    {
    };

    void load();
    void save();

    Units distanceUnits;
    Units pressureUnits;
    Units temperatureUnits;
    Units volumeUnits;
    uint8_t tachDutyCycle;
    uint8_t fuelOhmScale;

    uint32_t distanceTraveled;

    FuelData fuel;
private:
    static int get_buffer_index(int *pValue);
    static void rotate_buffer();
    static int get_data_buffer();
};


#endif

