#include <avr/interrupt.h>
#include "watchdog.h"

using namespace AVRAssist;


// PlatformIO sketch using AVRAssist to configure the
// watchdog timer to fire an interrupt every 2 seconds.
// The interrupt will toggle the Arduino's built in LED.

// Interrupt handler for the WDT interrupt.
ISR(WDT_vect) {
    PINB |= (1 << PINB5);
}


int main() {
    // Arduino D13 as OUTPUT pin.
    DDRB = (1 << DDB5);

    // The watchdog initialisation preserves the current
    // interrupt state, we need interrupts on.
    sei();

    Watchdog::initialise(
        Watchdog::WDT_TIMEOUT_2S,         // Fire every second.
        Watchdog::WDT_MODE_INTERRUPT      // Fire the WDT interrupt.
    );

    // The main loop does nothing.
    while (1) {
        ;
    }
}



