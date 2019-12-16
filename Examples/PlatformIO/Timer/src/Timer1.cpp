#include <avr/io.h>      
#include "timer1.h"

using namespace AVRAssist;


int main() {
    DDRB = (1 << DDB1);
    
    // Initialise Timer1 as CTC+OCR1A, divide by 256 and toggle OCRA (D9) on compare match.
    // OCR1A is set to 62499 to get about 1 second flash rate on an Arduino at 16 MHz
    // or t0 31249 on a breadboared "Arduino" at 8 MHz.

    // Then initialise Timer 1.
    Timer1::initialise( Timer1::MODE_CTC_OCR1A,             // Timer Mode
                        Timer1::CLK_PRESCALE_256,           // Clock Source
                        Timer1::OC1A_TOGGLE                 // Compare Match
                                                            // Remaining parameters default.
                       );

    // Required for a 1 second flash rate. The actual frequency of the timer
    // is 0.5 Hz and you need to toggle the LED twice to get one flash. 
    // Foca = F_CPU / (2 * prescaler * (1 + OCR1A))
    // Foca = 16E6 / (2 * 256 * 62499)
    // Foca = 16e6 / 512 * 62500
    // Foca = 16e6 / 32e6
    // Foca = 500 mHz = 0.5 Hz.

// Assume Arduino at 16 MHz.    
// or, Breadboard "Arduino" at 8 MHz.
#if F_CPU == 16000000L
    OCR1A = 62499;
#else
    OCR1A = 31249;
#endif

    while (1) {
        ;
    }    
}


