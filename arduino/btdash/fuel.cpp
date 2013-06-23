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

#include "Arduino.h"
#include "fuel.h"

void Fuel::init(Settings* pSettings, MultiMeter* pMeter)
{
    remainDistance = 0;
    remainTime = 0;
    remainPct = 0;
    remainVolume = 0;

    m_pSettings = pSettings;
    m_pMeter = pMeter;

    if (pSettings->fuelOhmScale == 255)
    {
        pSettings->fuelOhmScale = 20;

        // reset ohm segments to Ariel Atom 2 defaults
        pSettings->fuel.ohmSegments[0] = 106;
        pSettings->fuel.ohmSegments[1] = 90;
        pSettings->fuel.ohmSegments[2] = 35;
        pSettings->fuel.ohmSegments[3] = 6;
        pSettings->fuel.ohmSegments[4] = 4;

        // reset liters to Ariel Atom 2 defaults
        pSettings->fuel.litersSegments[0] = 5;
        pSettings->fuel.litersSegments[1] = 10;
        pSettings->fuel.litersSegments[2] = 20;
        pSettings->fuel.litersSegments[3] = 30;
        pSettings->fuel.litersSegments[4] = 40;
    }

    // prep for reading by pulling up the internal 10k resistor
    // on arduinos, you do this with a digitalWrite(HIGH) call on an analog IN pin
    digitalWrite(PIN_FUEL, HIGH);
}

// measures the fuel gauge values by reading ECU signals
void Fuel::measure()
{
    uint32_t ms = micros();
    if (ms > m_lastMeasured + FUEL_UPDATE_EVERY_MS)
    {
        m_lastMeasured = ms;
        FuelData* pFuelData = &m_pSettings->fuel;
        uint8_t* pLiterSegments = pFuelData->litersSegments;
        uint8_t* pOhmSegments = pFuelData->ohmSegments;

        ++pFuelData->nSamples;

        uint8_t ohms = (uint8_t)(m_pMeter->measureOhms(PIN_FUEL) / m_pSettings->fuelOhmScale);

        uint16_t lo = 0;
        uint16_t hi = 0;
        uint16_t dl = 0; // deciliters
        int iSegment;

        // determine which segment is the start of the level
        for (iSegment = FUEL_NUM_SEGMENTS - 1; iSegment >= 0; --iSegment)
        {
            lo = hi;
            hi = pOhmSegments[iSegment];

            if (ohms < hi)
            {
                dl = pLiterSegments[iSegment] * 10;
                if (hi > lo)
                {
                    // use a linear interpolation between the segments to get liters
                    dl += pLiterSegments[iSegment + 1] * 10 * (hi -ohms) / (hi - lo);
                }
            }
        }

        switch (m_pSettings->volumeUnits)
        {
        case UNITS_IMPERIAL:
            dl *= L_PER_GAL_IMPERIAL;
            break;
        case UNITS_US:
            dl *= L_PER_GAL_US;
            break;
        default:
            break;
        }

        remainVolume = dl;
        remainPct = dl * 10 / pLiterSegments[FUEL_NUM_SEGMENTS - 1];
    }
}

void Fuel::report()
{
    if (m_pSettings->debugMode)
    {
        SERIAL_BT.print(" fuelRaw=");
        SERIAL_BT.print(analogRead(PIN_FUEL));

        SERIAL_BT.print(" fuel%=");
        SERIAL_BT.print(remainPct);

        SERIAL_BT.print(" fuelV=");
        SERIAL_BT.print(remainVolume);

        //SERIAL_BT.print(", fuelD=");
        //SERIAL_BT.print(remainDistance);
        //SERIAL_BT.print(", fuelT=");
        //SERIAL_BT.print(remainTime);
    }
}