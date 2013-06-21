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

// reads EEPROM at EE_STATUS_BUFFER to get the current buffer offset
// see http://www.atmel.com/images/doc2526.pdf

#include "Arduino.h"
#include "settings.h"
#include <avr/eeprom.h>

int Settings::get_buffer_index(int *pValue)
{
    int i;
    int last;
    int cur = eeprom_read_byte((const unsigned char*)(EE_STATUS_BUFFER + EE_NUM_BUFFERS - 1));

    if (pValue)
        *pValue = 0;

    for (i = 0; i < EE_NUM_BUFFERS; ++i)
    {
        last = cur;
        cur = EE_READ(EE_STATUS_BUFFER + i);

        if (cur == last == 0 || abs(cur - last) > 1)
        {
            if (pValue)
                *pValue = last;
            break;
        }
    }

    return i % EE_NUM_BUFFERS;
}

// rotates the EEPROM data buffer to the next buffer slot to reduce EEPROM wear
void Settings::rotate_buffer()
{
    int value;
    int index = (get_buffer_index(&value) + 1) % EE_NUM_BUFFERS;
    EE_WRITE(EE_STATUS_BUFFER + index, value + 1);
}

// gets the address to the current EEPROM data buffer
int Settings::get_data_buffer()
{
    int index = get_buffer_index(NULL);
    return EE_DATA_BUFFERS + sizeof(EE_Data) * index;
}

// reads data and settings from EEPROM
void Settings::load()
{
    EE_Data data;
    byte* pData = (byte*)&data;
    int i;
    int address;

    for (i = 0, address = get_data_buffer(); i < sizeof(data); ++i, ++address, ++pData)
    {
        *pData = EE_READ(address);
    }

    this->distanceUnits    = (Units) (data.units & B00000011);
    this->pressureUnits    = (Units)((data.units & B00001100) >> 2);
    this->temperatureUnits = (Units)((data.units & B00110000) >> 4);
    this->volumeUnits      = (Units)((data.units & B11000000) >> 6);

    this->fuelOhmScale     = data.fuelOhmScale;
    this->tachDutyCycle    = data.tachDutyCycle;

    this->distanceTraveled = data.distanceTraveled;
    this->fuel             = data.fuel;
}

// writes data and settings to EEPROM.
void Settings::save()
{
    EE_Data data;
    byte* pData = (byte*)&data;
    int i;
    byte b1;
    byte b2;
    int address;
    int written = 0;

    // rotate the data buffer, then get the new address
    rotate_buffer();
    address = get_data_buffer();

    // pack settings and userdata into EE_Data
    data.distanceTraveled = this->distanceTraveled;
    data.fuel             = this->fuel;

    data.units = 
        (this->distanceUnits           & B00000011) |
        ((this->pressureUnits    << 2) & B00001100) |
        ((this->temperatureUnits << 4) & B00110000) |
        ((this->volumeUnits      << 6) & B11000000);

    data.fuelOhmScale   = this->fuelOhmScale;
    data.tachDutyCycle  = this->tachDutyCycle;

    // write to EEPROM any bytes that have changed
    for (i = 0; i < sizeof(EE_Data); ++i, ++address, ++pData)
    {
        EE_WRITE(address, *pData);
    }
}