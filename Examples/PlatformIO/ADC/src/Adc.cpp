#include <avr/interrupt.h>
#include "adc.h"
#include "timer1.h"

using namespace AVRAssist;

// Initialise the ADC.
void setupADC() {

    // Initialise the ADC in auto-trigger free running mode, with the
    // reference voltage on the AVCC pin and sample the voltage on ADC0
    // (Arduino A0, AVR PC0). Interrupts are on and the prescaler is 128.
    Adc::initialise(
        Adc::REFV_AVCC,             // ReferenceSource
        Adc::SAMPLE_ADC0,           // SampleSource
        Adc::INT_ENABLED,           // InterruptMode
        Adc::ALIGN_RIGHT,           // Alignment
        Adc::ADC_PRESCALE_128,      // Prescaler
        Adc::AUTO_ENABLED,          // AutoTriggerMode
        Adc::AUTO_FREE_RUNNING      // AutoTriggerSource
    );
}


// Initialise Timer1.
void setupTimer1() {
    // Timer1 is in Phase correct, 8 bit PWM with a
    // divide by 64 prescaler.
    Timer1::initialise(
        Timer1::MODE_PC_PWM_255,    // 8 bit phase correct PWM.
        Timer1::CLK_PRESCALE_64,    // Divide by 64.
        Timer1::OC1A_CLEAR          // Clear OC1A on match.
    );
}


// Arduino map() function.
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


// Somewhere for the ADC Interrupt to store the result.
volatile uint16_t ADCReading = 0;


// The interrupt handler.
ISR(ADC_vect) {
    ADCReading = ADCW;
}


int main() {
    // If you forget this next line, you will spend *hours*
    // trying to debug a simple program like this. How do I
    // know this? Guess!
    sei();

    // Initialise Timer1 in 8 bit PC PWM with divide by 64
    // clock prescaler.
    setupTimer1();

    // Initialise the ADC in free running mode with a divide by 128
    // clock prescaler using ADC0 (A0, PC0) as the sample voltage.
    setupADC();
    
    // Arduino pin D9 is AVR pin PB1.
    DDRB |= (1 << DDB1);

    // Now, fire up the ADC.
    Adc::start();

    while (1) { 
        // Adjust the brightness of the LED on PIN D9.
        // That gives 0 - 1023, we need 0 to 255.
        OCR1A = map(ADCReading, 0, 1023, 0, 255);
    }
}
