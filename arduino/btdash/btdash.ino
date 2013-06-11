#include <digitalWriteFast.h>


// By default, communicate at this bitrate. The listener client must match this value.
const unsigned long SERIAL_BPS = 115200;

#define VERSION_STRING "BTDASH_0.0.1"

// time constants for readability
const unsigned long MILLIS_PER_SECOND = 1000;
const unsigned long MICROS_PER_SECOND = 1000000;

// 33% duty cycle. For a 50% duty cycle, use 2, etc.
const unsigned int TACH_DUTY_CYCLE = 3;
const unsigned long MICROS_PER_RPM = 100 * MICROS_PER_SECOND / (3 * TACH_DUTY_CYCLE);

// input mapping
const unsigned int PIN_TACH = 52;

// output mapping
#define SERIAL_BT Serial

unsigned long duration;
// word size is 16 bits, so max RPM is 32767. If your engine revs that high, you're driving an F1 from 2154!
unsigned int uRpm = 0;
unsigned long ulRpmTrigger = 0;

// the setup routine runs once when you power on or press reset
void setup()  { 
  // initialize the serial output
  SERIAL_BT.begin(SERIAL_BPS);

  pinMode(PIN_TACH, INPUT);
  establishContact();
} 

// the loop routine runs over and over again forever
void loop() {
  readTach();
  writeDash();

  delay(500);
}

void establishContact() {
  while (SERIAL_BT.available() <= 0) {
    SERIAL_BT.println(VERSION_STRING); // send an initial string
    delay(500);
  }
}

//
// write the current state of the instruments to the serial output.
//
void writeDash() {
  if (SERIAL_BT.available() > 0) {
    SERIAL_BT.print("rpm=");
    SERIAL_BT.print(uRpm);

    SERIAL_BT.println();
  }
}

//
// read the tachometer feed from the car's ECU
//
void readTach() {
  // On the Ariel Atom 2 w/ SPA dash, the tach signal is a 12V pulse with a 33% duty cycle on pin 1 of the SPA harness.
  if (digitalRead(PIN_TACH) == LOW) {
    if (ulRpmTrigger != 0) {
      uRpm = MICROS_PER_RPM / (micros() - ulRpmTrigger);

      // reset the trigger
      ulRpmTrigger = 0;
    }
  }
  else if (ulRpmTrigger == 0) {
    // transitioning from low to high.
    ulRpmTrigger = micros();
  }
}



