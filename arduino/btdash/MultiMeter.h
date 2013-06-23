#pragma once

#include "Arduino.h"
#include "binary.h"

class MultiMeter
{
public:
    MultiMeter(void)
    {
        m_pulledUp = 0;

        // assume the interval 5v source as the pullup
        Vin = 5.0;
    }

    // measures the resistance on a circuit like this:
    //
    // 5v------10k---------R2------GND
    //          |
    //        Analog
    uint32_t measureOhms(uint8_t pin)
    {
        if (validAnalogPin(pin))
        {
            ensurePulledUp(pin);
            int raw = analogRead(pin);
            Vout = (5.0 / 1023.0) * raw;
            R2 = 10000.0 / (5.0 / Vout) - 1;
        }
        return (uint32_t)R2;
    }

    ~MultiMeter(void)
    {
    }

private:
    void ensurePulledUp(uint8_t pin)
    {
        if (m_pulledUp & (1 << (pin - A0)) == 0)
        {
            digitalWrite(pin, HIGH);
        }
    }

    inline bool validAnalogPin(uint8_t pin)
    {
        #if (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__))
        return pin - A0 < 16;
        #else
        return pin - A0 < 8;
        #endif
    }


    // support up to 16 pins for analog
    uint16_t m_pulledUp;

    float Vin;

    float Vout;
    float R1;
    float R2;
};

