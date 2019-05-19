#include "timer1.h"

using namespace AVRAssist;

#define LED 9      // LED on OCRA = D9.

void setup() {
    // LED is an output pin.
    pinMode(LED, OUTPUT);

    // Initialise Timer1 as CTC+OCR1A, divide by 256 and toggle OCRA (D9) on compare match.
    // OCR1A ia set to 31249 to get about 1 second flash rate.

    // Do this here and the LED goes off at 16.32 KHz and appears always on
    // This doesn't happen with PlatformIO.
    // OCR1A = 31249;

    // Then initialise Timer 1.
    Timer1::initialise( Timer1::MODE_CTC_OCR1A,             // Timer Mode
                        Timer1::CLK_PRESCALE_256,           // Clock Source
                        Timer1::OC1A_TOGGLE                 // Compare Match
                                                            // Remaining parameters default.
                       );
     OCR1A = 31249;
}


void loop() {
    ; // Do nothing, the timer controls the flashing LED.
}


