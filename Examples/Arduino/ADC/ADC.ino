#include "adc.h"

using namespace AVRAssist;

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


// Somewhere for the ADC Interrupt to store the result.
volatile uint16_t ADCReading = 0;


// The interrupt handler.
ISR(ADC_vect) {
    ADCReading = ADCW;
}


void setup() {
    setupADC();
    
    // Use the Serial monitor for output.
    Serial.begin(9600);
    Serial.println("Arduino Direct ADC Testing");

    pinMode(9, OUTPUT);

    // Now, fire up the ADC.
    Adc::start();
}


void loop() { 

    Serial.print("ADC = ");
    Serial.print(ADCReading);

    // The voltage is ADCReading * (5V/1024)
    Serial.print(", Voltage = ");
    Serial.println((float)(ADCReading * 5.0 / 1024.0));

    // Adjust the brightness of the LED on PIN D9.
    analogWrite(9, map(ADCReading, 0, 1023, 0, 255));

}
