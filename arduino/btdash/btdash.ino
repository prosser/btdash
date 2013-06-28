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

#include "SimTach.h"

typedef enum Commands
{
    Command_None = 0,
    Command_SetMode,
    Command_SimSetTachRpm,
    Command_SimSetTachMicrosPerRpm,
    Command_Pause,
    Command_Step,
} Commands;

uint32_t duration;
Settings settings;
Instruments instruments;
String serialBuffer;
boolean serialReady;
Commands command;
MultiMeter meter;

SimTach simTach(PIN_TACH_SIM);

#if defined(PROGRAMMING_EEPROM)
#include <avr/eeprom.h>
#endif

//
// write the current state of the instruments to the serial output.
//

// the setup routine runs once when you power on or press reset
void setup()
{
    command = Command_None;
    serialReady = false;
    serialBuffer.reserve(100);

    // initialize the serial output
    SERIAL_BT.begin(SERIAL_BPS, SERIAL_8N1);
    pinMode(PIN_TACH, INPUT);
    pinMode(PIN_FUEL, INPUT);

    SERIAL_BT.println("Initializing...");

    instruments.init(&settings, &meter);
    simTach.init(&settings);

#if defined(PROGRAMMING_EEPROM)
    // don't do this a lot! this programs the EEPROM
    unsigned long i = 0;
    while (i < EE_SIZE) {
        SERIAL_BT.print('.');
        eeprom_write_byte((uint8_t*)i++, 0);
    }

    SERIAL_BT.println("done");
#endif

    // start with fairly sane defaults
    settings.load();
    instruments.tach.rpm = 800;

#if defined(PROGRAMMING_EEPROM)
    SERIAL_BT.print("Programming default settings to EEPROM...");
    settings.fuelOhmScale = 1;
    settings.mode = MODE_SIGNAL_PROCESSING;
    settings.debugMode = true;
    settings.pressureUnits = UNITS_US;
    settings.volumeUnits = UNITS_US;
    settings.temperatureUnits = UNITS_US;
    settings.distanceUnits = UNITS_US;
    settings.tachDutyCycle = 3;
    settings.distanceTraveled = 10060;
    settings.save();
    SERIAL_BT.println("done");
#endif
}
// the loop routine runs over and over again forever
void loop()
{
    boolean wait = false;
    if (serialReady)
    {
        switch (command)
        {
        case Command_SetMode:
            switch (serialBuffer.charAt(0))
            {
            case '1':
                settings.mode = MODE_SIGNAL_GENERATION;
                SERIAL_BT.println("*****\n*****\n*****\nSIGNAL GENERATION");
                command = Command_None;
                break;

            case '2':
                settings.mode = MODE_SIGNAL_PROCESSING;
                SERIAL_BT.println("*****\n*****\n*****\nSIGNAL PROCESSING");
                command = Command_None;
                break;
            }
            break;

        case Command_Pause:
            wait = true;
            // fallthrough
        case Command_None:
            switch (serialBuffer.charAt(0))
            {
            case '\0':
                command = Command_None;
                break;

            case 'p':
            case 'P':
                SERIAL_BT.print("> ");
                command = Command_Pause;
                break;

            case 's':
            case 'S':
                SERIAL_BT.print("> ");
                wait = true;
                break;

            case 'M':
            case 'm':
                SERIAL_BT.print("Set mode:\n1. Signal generation\n2. Signal processing\n> ");
                command = Command_SetMode;
                break;
            case 'r':
            case 'R':
                if (settings.mode == MODE_SIGNAL_GENERATION)
                {
                    SERIAL_BT.println("tachometer RPMs...");
                    command = Command_SimSetTachRpm;
                }
                break;
            case 'k':
            case 'K':
                if (settings.mode == MODE_SIGNAL_GENERATION)
                {
                    SERIAL_BT.println("micros per RPM [k] ...");
                    command = Command_SimSetTachMicrosPerRpm;
                }
                break;
            default:
                // do nothing
                break;
            }
            break;

        case Command_SimSetTachRpm:
            {
                instruments.tach.rpm = min(abs(serialBuffer.toInt()), TACH_MAX_RPM);
                SERIAL_BT.print("Setting tach output to ");
                SERIAL_BT.print(instruments.tach.rpm);
                SERIAL_BT.println(" RPM");
                command = Command_None;
            }
            break;
        case Command_SimSetTachMicrosPerRpm:
            {
                simTach.microsPerRpm = serialBuffer.toInt();
                SERIAL_BT.print("Setting tach microsPerRpm factor (r) to ");
                SERIAL_BT.println(simTach.microsPerRpm);
                command = Command_None;
            }
            break;
        }
        serialBuffer = "";
        serialReady = false;
    }

    if (!wait)
    {
        switch (settings.mode)
        {
        case MODE_SIGNAL_PROCESSING:
            instruments.measure();
            instruments.report();
            break;

        case MODE_SIGNAL_GENERATION:
            simTach.simulate(&instruments.tach);
            break;
        }
    }
}

void serialEvent()
{
    while (SERIAL_BT.available())
    {
        char c = (char)SERIAL_BT.read();

        switch (c)
        {
        case '\n':
            serialReady = true;
            break;

        case '\r':
        case '\t':
            break;

        default:
            serialBuffer += c;
            break;
        }
    }
}