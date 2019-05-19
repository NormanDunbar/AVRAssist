#ifndef __COMPARATOR_H__
#define __COMPARATOR_H__

// The following should allow the Arduino IDE or not, to
// use this header file.
#if defined(ARDUINO)
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "WProgram.h"
    #endif
#else
    #include <avr/io.h>
#endif

namespace AVRAssist {
    
    //----------------------------------------------------------------------
    // Analogue Comparator setup.
    //----------------------------------------------------------------------
    namespace Comparator {
        
        //------------------------------------------------------------------
        // Where do we get our reference voltage from? Either pin AIN0 or
        // the internal bandgap reference voltage.
        //------------------------------------------------------------------
        enum reference_t : uint8_t {
            REFV_EXTERNAL = 0,    // Pin AIN0, Arduino pin D6.
            REFV_INTERNAL
        };
        
        //------------------------------------------------------------------
        // Where do we get our sampled voltage from? Either pin AIN1 or any
        // of the ADC channels 0 through 7, but not the internal channel 8.
        //------------------------------------------------------------------
        enum sample_t  : uint8_t {
            SAMPLE_ADC0 = 0,
            SAMPLE_ADC1,
            SAMPLE_ADC2,
            SAMPLE_ADC3,
            SAMPLE_ADC4,
            SAMPLE_ADC5,
            SAMPLE_ADC6,
            SAMPLE_ADC7,
            SAMPLE_AIN1         // Pin AIN1, Arduino pin D7.
        };
        
        //------------------------------------------------------------------
        // If we require interrupts, what variation do we need/want?
        //------------------------------------------------------------------
        enum interrupt_t  : uint8_t {
            INT_NONE = 0,
            INT_TOGGLE = (1 << ACIE),                   //  8 aka 1000
            INT_FALLING = INT_TOGGLE | (1 << ACIS1),    // 10 aka 1010
            INT_RISING = INT_FALLING | (1 << ACIS0)     // 11 aka 1011
        };

        //------------------------------------------------------------------
        // Initialise the analogue comparator with a reference voltage
        // source, a comparison voltage source and any required interrupts.
        //------------------------------------------------------------------
        void initialise(const reference_t referenceSource, 
                        const sample_t sampleSource, 
                        const interrupt_t interruptMode = INT_NONE) {

            //--------------------------------------------------------------
            // Validation...
            //--------------------------------------------------------------
            if (referenceSource != REFV_EXTERNAL && referenceSource != REFV_INTERNAL) {
                    return;
            }
            
            if (sampleSource > SAMPLE_AIN1) {
                    return;
            }

            // In case someone ORs them together. It happens!
            if (interruptMode != INT_NONE && interruptMode != INT_TOGGLE &&
                interruptMode != INT_FALLING && interruptMode != INT_RISING) {
                return;
            }
            
            
            //--------------------------------------------------------------
            // Initial enabling of the Analogue Comparator.
            //--------------------------------------------------------------
            DIDR1 = 0;              // AIN0/AIN1 (D6/D7) still used for I/O.

            // Disable AC interrupts.
            ACSR &= ~(1 << ACIE);

            // Enable AC by clearing the AC Disable bit. Disable the disabler?
            ACSR &= ~(1 << ACD);


            //--------------------------------------------------------------
            // Reference voltage source...
            //--------------------------------------------------------------
            if (referenceSource == REFV_EXTERNAL) {
                // Pin AIN0 aka D6. Disable D6 I/O and the internal 
                // bandgap reference.
                DIDR1 |= (1 << AIN0D);
                ACSR &= ~(1 << ACBG);
            } else {
                // Internal bandgap reference.
                ACSR |= (1 << ACBG);        
            }
            
            //--------------------------------------------------------------
            // Sampled voltage source...
            //--------------------------------------------------------------
            if (sampleSource == SAMPLE_AIN1) {
                // Pin AIN1 aka D7. Disable D7 I/O and enable AIN1.
                //
                // This is the short version, the long versions is:
                // DIDR1 |= (1 << AIN1D);
                // ADCSRB |= (1 << ACME);
                // ADCSRA |= (1 << ADEN);
                //
                DIDR1 |= (1 << AIN1D);
                ADCSRB &= ~(1 << ACME);
            } else {
                // One of the ADC MUX inputs 0 through 7.
                // Power up the ADC, disable the MUX from 
                // the ADC, enable the MUX for the AC and
                // select the MUX input to use.
                PRR &= ~(1 << PRADC);
                ADCSRA &= ~(1 << ADEN);
                ADCSRB |= (1 << ACME);
                ADMUX &= 0xf0;
                ADMUX |= sampleSource;
            }

            //--------------------------------------------------------------
            // Interrupts...
            // ADIE is already disabled, so INT_DISABLED has no effect.
            //--------------------------------------------------------------
            ACSR &= 0xF0;
            ACSR |= interruptMode;
        }

    } // End of comparator namespace.
  
}  // End of AVRAssist namespace.
#endif // __COMPARATOR_H__
