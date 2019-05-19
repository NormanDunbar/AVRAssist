#include "comparator.h"

using namespace AVRAssist;

void setup() {
    // You can still use Arduino code as well -
    // but I'm not!
    DDRB |= (1 << DDB5);  //pinMode(LED_BUILTIN, OUTPUT);
    Comparator::initialise(
        Comparator::REFV_EXTERNAL,      // Reference voltage on AIN0/D6.
        Comparator::SAMPLE_AIN1,        // Sample voltage on AIN1/D7.
        Comparator::INT_TOGGLE);        // Fire interrupt when ACO changes.
}


void loop() {
    // You could do something meaningful here ....
    ;   // I'm doing nothing, just looping.
}


// Analogue Comparator Interrupt Handler.Reads the ACSR
// register and sets the LED to the state of the ACO bit.
ISR(ANALOG_COMP_vect) {
    if (ACSR & (1 << ACO)) {
        PORTB |= (1 << PORTB5);   // digitalWrite(LED_BUILTIN, HIGH);
    } else {
        PORTB &= ~(1 << PORTB5);  // digitalWrite(LED_BUILTIN, LOW);
    }
    
}
