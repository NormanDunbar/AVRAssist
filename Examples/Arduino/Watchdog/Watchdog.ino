#include "watchdog.h"

using namespace AVRAssist;

// Arduino sketch using AVRAssist to configure the
// watchdog timer to fire an interrupt every 1 second.
// The interrupt will toggle the Arduino's built in LED.

void setup() {
    // LED_BUILTIN is an output pin.
    pinMode(LED_BUILTIN, OUTPUT);

     Watchdog::initialise(
        Watchdog::WDT_TIMEOUT_1S,         // Fire every second.
        Watchdog::WDT_MODE_INTERRUPT      // Fire the WDT interrupt.
    );
}


void loop() {
  while (true) {
    ;   // Do nothing.
  }
}

// Interrupt handler. Called every second.
// Toggles the LED on Arduino pin D13. (Built in LED.)
ISR(WDT_vect) {
  PINB |= (1 << PINB5);
}
