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
#include "Instruments.h"

void Instruments::init(Settings* pSettings)
{
    tach.init(pSettings);
    odo.init(pSettings);
    fuel.init(pSettings, &m_meter);
}

void Instruments::measure()
{
    tach.measure();
    odo.measure();
    fuel.measure();
    speedo.measure();
}

void Instruments::report()
{
    if (SERIAL_BT.available() > 0)
    {
        unsigned long now = millis();
        if (now - m_lastReported >= 500)
        {
            m_lastReported = now;

            tach.report();
            odo.report();
            fuel.report();
            speedo.report();

            SERIAL_BT.clearWriteError();
            SERIAL_BT.flush();

            SERIAL_BT.write("Instruments");
            SERIAL_BT.write(sizeof(Instruments));
            SERIAL_BT.write((uint8_t*)this, sizeof(Instruments));

            SERIAL_BT.flush();
        }
    }
}