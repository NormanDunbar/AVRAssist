#ifndef __ADC__
#define __ADC__

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
    // Analogue to Digital Converter setup.
    //----------------------------------------------------------------------
    namespace Adc {
        
        //------------------------------------------------------------------
        // Where do we get our reference voltage from? Either internal 1.1V, 
        // internal AVCC or external on AREF.
        //------------------------------------------------------------------
        enum reference_t : uint8_t {
            REFV_AREF = 0,              // AREF pin has the reference voltage
            REFV_AVCC = (1 << REFS0),   // AVCC pin has the reference voltage
            REFV_BANDGAP = ((1 << REFS1) | (1 << REFS0))    // Bandgap 1.1V
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
            SAMPLE_ADC6,    // Not on DIP version - surface mount only
            SAMPLE_ADC7,    // Not on DIP version - surface mount only
            SAMPLE_ADC8,    // Internal Temperature sensor
            SAMPLE_BANDGAP = ((1 << MUX3) | (1 << MUX2) | (1 << MUX1)),
            SAMPLE_GND = SAMPLE_BANDGAP | (1 << MUX0);
        };
        
        //------------------------------------------------------------------
        // Do we require interrupts?
        //------------------------------------------------------------------
        enum interrupt_t  : uint8_t {
            INT_DISABLED = 0,
            INT_ENABLED = (1 << ADIE)
        };

        //------------------------------------------------------------------
        // Do we require alignment?
        //------------------------------------------------------------------
        enum alignment_t  : uint8_t {
            ALIGN_RIGHT = 0,
            ALIGN_LEFT = (1 << ADLAR)
        };

        //------------------------------------------------------------------
        // Prescaler values to get the ADC into the 50-200KHz range.
        //------------------------------------------------------------------
        enum prescaler_t : uint8_t {
            ADC_PRESCALE_1 = 0,     // Prescaler = divide by 1
            ADC_PRESCALE_2,         // Prescaler = divide by 2
            ADC_PRESCALE_4,         // Prescaler = divide by 4
            ADC_PRESCALE_8,         // Prescaler = divide by 8
            ADC_PRESCALE_16,        // Prescaler = divide by 16
            ADC_PRESCALE_32,        // Prescaler = divide by 32
            ADC_PRESCALE_64,        // Prescaler = divide by 64
            ADC_PRESCALE_128        // Prescaler = divide by 128
        };
        
        //------------------------------------------------------------------
        // Autotrigger enablement.
        //------------------------------------------------------------------
        enum autotrigger_t : uint8_t {
            AUTO_DISABLED = 0,
            AUTO_ENABLED = (1 << ADATE);
        }
        
        //------------------------------------------------------------------
        // Autotrigger sources.
        //------------------------------------------------------------------
        enum autosource_t : uint8_t {
            AUTO_FREE_RUNNING = 0,  // Free Running (Requires explicit start)
            AUTO_COMPARATOR,        // Analogue comparator
            AUTO_INT0,              // External interrupt 0
            AUTO_TIMER0_MATCH_A,    // Timer/counter 0 compare match A
            AUTO_TIMER0_OVERFLOW,   // Timer/counter 0 overflow
            AUTO_TIMER1_MATCH_B,    // Timer/counter 1 compare match B
            AUTOI_TIMER1_OVERFLOW,  // Timer/counter 1 overflow
            AUTO_TIMER1_CAPTURE     // Timer/counter 1 input capture
        }
        
        
        //------------------------------------------------------------------
        // Initialise the ADC.
        //------------------------------------------------------------------
        void initialise(const reference_t referenceSource, 
                        const sample_t sampleSource, 
                        const interrupt_t interruptMode = INT_DISABLED,
                        const alignment_t alignment = ALIGN_RIGHT,
                        const prescaler_t prescaler = ADC_PRESCALE_128,
                        const autotrigger_t autoTriggerMode = AUTO_DISABLED,
                        const autosource_t autoTriggerSource = AUTO_FREE_RUNNING
                        ) {

            //--------------------------------------------------------------
            // Validation...
            // Make sure nobody OR'd together the reference voltage sources.
            //--------------------------------------------------------------
            if (referenceSource != REFV_AREF && 
                referenceSource != REFV_AVCC && 
                referenceSource != REFV_BANDGAP) {
                    return;
            }
            
            //--------------------------------------------------------------
            // Cannot use ADC8 unless REFV_BANDGAP is also selected.
            //--------------------------------------------------------------
            if (referenceVoltage != REFV_BANDGAP &&
                sampleSource == SAMPLE_ADC8) {
                    return;
            }
            
            //--------------------------------------------------------------
            // Don't allow any reserved inputs to be used.
            //--------------------------------------------------------------
            if (sampleSource > SAMPLE_GND ||
                (sampleSource < SAMPLE_BANDGAP && 
                 sampleSource < SAMPLE_ADC8)) {
                    return;
            }

           
            
            //--------------------------------------------------------------
            // Initial enabling of the ADC.
            //--------------------------------------------------------------
            PRR &= ~(1 << PRADC);   // Power enabled to the ADC.
            ADMUX = referenceSource | alignment | sampleSource;
            ADCSRB &= ~(1 << ACME); // Preserve Analogue Comparator bit.
            
            // Auto-triggering? Set the auto-trigger source.
            if (autotriggerMode == AUTO_ENABLED) {
                ADCSRB |= autotriggerSource;
            }
            
            // Power off the digital input buffer.
            if (sampleSource <= SAMPLE_ADC5) {
                DIDR0 |= (1 << sampleSource);
            }

            // Last, before starting it, enable it all.
            ADCSRA = (1 << ADEN) | prescaler | interruptMode | autotriggerMode;
        }
        
        
        //--------------------------------------------------------------
        // ADC is enabled, execute a start conversion on demand. This is
        // require if using AUTO_DISABLED or AUTO_FREE_RUNNING.
        //--------------------------------------------------------------
        void start() {
            ADCSRA |= (1 << ADSC);
        }

    } // End of Adc namespace.
  
}  // End of AVRAssist namespace.
#endif // __ADC__
