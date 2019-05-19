#include <avr/io.h>      
#include "timer1.h"

using namespace AVRAssist;


int main() {
    DDRB = (1 << DDB1);
    
    // Initialise Timer1 as CTC+OCR1A, divide by 256 and toggle OCRA (D9) on compare match.
    // OCR1A ia set to 31249 to get about 1 second flash rate.

    // Then initialise Timer 1.
    Timer1::initialise( Timer1::MODE_CTC_OCR1A,             // Timer Mode
                        Timer1::CLK_PRESCALE_256,           // Clock Source
                        Timer1::OC1A_TOGGLE                 // Compare Match
                                                            // Remaining parameters default.
                       );

    // Required for a 1 second flash rate. The actual frequency of the timer
    // is 1 Hz and as you need to toggle the LED twice to get one flash, there's
    // a 2 multiplier in the following.
    // Foca = F_CPU / (2 * prescaler * (1 + OCR1A))
    // Foca = 16E6 / (2 * 256 * 31250)
    // Foca = 16e6 / 512 * 31250
    // Foca = 16e6 / 16e6
    // Foca = 1 Hz.
    
    OCR1A = 31249;

    while (1) {
        ;
    }    
}


