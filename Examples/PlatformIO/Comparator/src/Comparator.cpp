#include <avr/interrupt.h>
#include "comparator.h"

using namespace AVRAssist;

// Analogue Comparator Interrupt Handler.Reads the ACSR
// register and sets the LED to the state of the ACO bit.
ISR(ANALOG_COMP_vect) {
    if (ACSR & (1 << ACO)) {
        PORTB |= (1 << PORTB5);   // HIGH
    } else {
        PORTB &= ~(1 << PORTB5);  // LOW
    }
    
}



int main(void)
{
    // SETUP
    // PB5/D13 is an output pin.
    DDRB |= (1 << DDB5);

    // Initialise the comparator with reference on AIN0/D6,
    // sample on AIN1/D7 and fire the interrupt when ACO
    // toggles from HIGH to LOW or LOW to HIGH.
    Comparator::initialise(
        Comparator::REFV_EXTERNAL,      // Reference voltage on AIN0/D6.
        Comparator::SAMPLE_AIN1,        // Sample voltage on AIN1/D7.
        Comparator::INT_TOGGLE);        // Fire interrupt when ACO changes.

    // Interrupts are required.
    sei();

    
    // LOOP
    while (1) {
        // Do nothing...
        ;
    }
}

