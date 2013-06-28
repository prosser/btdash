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

#include "units.h"
#include "tach.h"

void Tachometer::init(Settings* pSettings)
{
    m_pSettings = pSettings;

    m_lastMeasured = micros();
    rpm = 0;

    // this is what it looks like for my 4-cylinder car. YMMV.
    microsPerRpm = MICROS_PER_SECOND * 7 / (pSettings->tachDutyCycle * 3) / 90;
}

//
// read the tachometer feed from the car's ECU and updates a Tachometer struct
//
void Tachometer::measure()
{
    if (digitalRead(PIN_TACH) == LOW) {
        if (m_lastMeasured != 0) {
            rpm = microsPerRpm / (micros() - m_lastMeasured);

            // reset the measure timestamp
            m_lastMeasured = 0;
        }
    }
    else if (m_lastMeasured == 0) {
        // transitioning from low to high.
        m_lastMeasured = micros();
    }
}

void Tachometer::report()
{
    if (m_pSettings->debugMode)
    {
        SERIAL_BT.print(" rpm=");
        SERIAL_BT.print(rpm);
    }
}