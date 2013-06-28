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

#include "SimTach.h"

uint16_t SimTach::m_rpmTest[20] = { 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 0, 9000 };

SimTach::SimTach(int pin)
{
    m_pin = pin;
}

void SimTach::init(Settings* pSettings)
{
    m_pSettings = pSettings;
    pinMode(m_pin, OUTPUT);
    microsPerRpm = 9700000;
    m_testIndex = 19;
    m_lastTestMillis = millis();
}

SimTach::~SimTach(void)
{
}

// simulates a tach signal by running through the values in m_rpmTest and sending a signal on the out pin
void SimTach::simulate(Tachometer *pTach)
{
    unsigned long now = millis();
    if (now - m_lastTestMillis > 1000)
    {
        m_testIndex = (m_testIndex + 1) % 20;
        pTach->rpm = m_rpmTest[m_testIndex];
        m_lastTestMillis = now;
    }

    if (pTach->rpm == 0)
    {
        pTach->rpm = 1;
    }

    unsigned long mh = microsPerRpm / pTach->rpm;
    unsigned long ml = mh * (m_pSettings->tachDutyCycle - 1);
    unsigned long ms;

    if (m_lastRpm != pTach->rpm ||
        m_lastMicrosPerRpm != microsPerRpm)
    {
        m_lastRpm = pTach->rpm;
        m_lastMicrosPerRpm = microsPerRpm;

        if (m_pSettings->debugMode)
        {
            SERIAL_BT.print("rpm: ");
            SERIAL_BT.print(pTach->rpm);

            SERIAL_BT.print(" microsPerRpm: ");
            SERIAL_BT.print(microsPerRpm);

            SERIAL_BT.print(" high: ");
            SERIAL_BT.print(mh);
            SERIAL_BT.print(" low: ");
            SERIAL_BT.println(ml);
        }
    }

    digitalWrite(m_pin, HIGH);
    if (mh > 1000)
    {
        ms = mh / 1000;
        delay(min(20, ms));
        delayMicroseconds(mh - (ms * 1000));
    }
    else
    {
        delayMicroseconds(mh);
    }

    digitalWrite(m_pin, LOW);
    if (ml > 1000)
    {
        ms = ml / 1000;
        delay(min(20, ms));
        delayMicroseconds(ml - (ms * 1000));
    }
    else
    {
        delayMicroseconds(ml);
    }
}