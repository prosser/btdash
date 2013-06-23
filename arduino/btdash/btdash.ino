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


#include "settings.h"
#include "Instruments.h"

uint32_t duration;
Settings settings;
Instruments instruments;

//
// write the current state of the instruments to the serial output.
//
void dash_update() {
    if (SERIAL_BT.available() > 0) {
        SERIAL_BT.print("rpm=");
        SERIAL_BT.print(instruments.tach.rpm);

        SERIAL_BT.print(", fuel%=");
        SERIAL_BT.print(instruments.fuel.remainPct);
        SERIAL_BT.print(", fuelD=");
        SERIAL_BT.print(instruments.fuel.remainDistance);
        SERIAL_BT.print(", fuelT=");
        SERIAL_BT.print(instruments.fuel.remainTime);

        SERIAL_BT.println();
    }
}

// the setup routine runs once when you power on or press reset
void setup()  { 
    // initialize the serial output
    SERIAL_BT.begin(SERIAL_BPS);
    pinMode(PIN_TACH, INPUT);
    pinMode(PIN_FUEL, INPUT);

    // establish contact
    while (SERIAL_BT.available() <= 0)
    {
        SERIAL_BT.println(VERSION_STRING); // send an initial string
        delay(500);
    }

    instruments.init(&settings);
} 

// the loop routine runs over and over again forever
void loop() {
    instruments.update();
    dash_update();
    delay(500);
}
