#ifndef __TIMER1_H__
#define __TIMER1_H__

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
    // Timer 1 setup
    //----------------------------------------------------------------------
    namespace Timer1 {
        
        //------------------------------------------------------------------
        // TIMER MODES. Mode 13 is reserved and not used.  The rest are
        // simply used to index into the timerModes[][] array below.
        //------------------------------------------------------------------
        enum  : uint8_t {
            MODE_NORMAL = 0,                        // Mode 0: Normal mode.                        
            MODE_PC_PWM_255,                        // Mode 1: Phase Correct PWM, TOP = 255
            MODE_PC_PWM_511,                        // Mode 2: Phase Correct PWM, TOP = 511
            MODE_PC_PWM_1023,                       // Mode 3: Phase Correct PWM, TOP = 1,023
            MODE_CTC_OCR1A,                         // Mode 4: CTC, TOP = OCR1A.
            MODE_FAST_PWM_255,                      // Mode 5: Fast PWM, TOP = 255.
            MODE_FAST_PWM_511,                      // Mode 6: Fast PWM, TOP = 511.
            MODE_FAST_PWM_1023,                     // Mode 7: Fast PWM, TOP = 1,023.
            MODE_PC_FC_PWM_ICR1,                    // Mode 8: Phase & Frequency Correct  PWM, TOP = ICR1.
            MODE_PC_FC_PWM_OCR1A,                   // Mode 9: Phase & Frequency Correct  PWM, TOP = OCR1A.
            MODE_PC_PWM_ICR1,                       // Mode 10: Phase Correct PWM, TOP = ICR1.
            MODE_PC_PWM_OCR1A,                      // Mode 11: Phase Correct PWM, TOP = OCR1A.
            MODE_CTC_ICR1,                          // Mode 12: CTC, TOP = ICR1.
            MODE_RESERVED_13,                       // Mode 13: Reserved, do not use.
            MODE_FAST_PWM_1CR1,                     // Mode 14: Fast PWM, TOP = ICR1.
            MODE_FAST_PWM_OCR1A                     // Mode 15: Fast PWM, TOP = OCR1A.
        };
        
        //------------------------------------------------------------------
        // TCCR1A and TCCR1B initial settings for the various timer modes. 
        // TCCR1A is index [n][0]
        // TCCR1B is index [n][1]
        // These bits end up in WGM11 & WGM10 in TCCR1A and WGM13 & WGM02 in 
        // TCCR1B depending on the timer mode chosen from the above.
        //------------------------------------------------------------------
        const uint8_t timerModes[16][2] = {                 // [n][0] = TCCR1A, [n][1] = TCCR1B
            {0, 0},                                         // Normal, TOP = 65,535
            {(1 << WGM10), 0},                              // Phase Correct PWM - 8 bit, TOP = 255
            {(1 << WGM11), 0},                              // Phase Correct PWM - 9 bit, TOP = 511
            {((1 << WGM10) | (1 << WGM11)), 0},             // Phase Correct PWM - 10 bit, TOP = 1,023
            {0, (1 << WGM12) },                             // Clear Timer on Compare, TOP = OCR1A
            {(1 << WGM10), (1 << WGM12)},                   // Fast PWM 8 bit, TOP = 255
            {(1 << WGM11), (1 << WGM12)},                   // Fast PWM 9 bit, TOP = 511
            {((1 << WGM10) | (1 << WGM11)), (1 << WGM12)},  // Fast PWM 10 bit- TOP = 1,023
            {0, (1 << WGM13)},                              // Phase & Frequency Correct PWM, TOP = ICR1    
            {(1 << WGM10), (1 << WGM13)},                   // Phase & Frequency Correct PWM, TOP = OCR1A    
            {(1 << WGM11), (1 << WGM13)},                   // Phase Correct PWM, TOP = ICR1A    
            {((1 << WGM10) | (1 << WGM11)), (1 << WGM13)},  // Phase Correct PWM, TOP = OCR1A
            {0, ((1 << WGM12) | (1 << WGM13))},             // Clear Timer on Compare, TOP = ICR1    
            {(1 << WGM10), ((1 << WGM12) | (1 << WGM13))},  // Reserved, don't use.
            {(1 << WGM11), ((1 << WGM12) | (1 << WGM13))},  // Fast PWM, TOP = ICR1
            {((1 << WGM10) | (1 << WGM11)), ((1 << WGM12) | (1 << WGM13))}, // Fast PWM, TOP = OCR1A
        };  // end of Timer1::Modes
      
        //------------------------------------------------------------------
        // CLOCK SOURCES. Note that external source, pin 'T1' is 
        // physical pin 11, Arduino pin D5 or AVR pin PD5.
        //------------------------------------------------------------------
        enum clockSource_t : uint8_t  {
            CLK_DISABLED,                           // No clock running
            CLK_PRESCALE_1,                         // Prescaler = divide by 1
            CLK_PRESCALE_8,                         // Prescaler = divide by 8
            CLK_PRESCALE_64,                        // Prescaler = divide by 64
            CLK_PRESCALE_256,                       // Prescaler = divide by 256
            CLK_PRESCALE_1024,                      // Prescaler = divide by 1024
            CLK_T1_FALLING,                         // External pin T0, falling edge
            CLK_T1_RISING                           // External pin T0, rising edge
        };

        //------------------------------------------------------------------
        // COMPARE MATCH bits.
        // What happens when OCR1A, OCR1B match TCNT1? 
        // Pin OC1A = Physical pin 17, Arduino pin D11 or AVR pin PB3.
        // Pin OC1B = Physical pin 5, Arduino pin D3 or AVR pin PD3.
        //------------------------------------------------------------------
        enum compareMatch_t  : uint8_t {
            OC1X_DISCONNECTED = 0,                  // Nothing - OC1A, OC1B both disconnected.
            OC1A_TOGGLE  = (1 << COM1A0),           // OC1A will toggle.
            OC1A_CLEAR   = (1 << COM1A1),           // OC1A will clear.
            OC1A_SET     = (1 << COM1A0) | (1 << COM1A1),   // OC1A will be set.
            OC1B_TOGGLE  = (1 << COM1B0),           // OC1B will toggle.
            OC1B_CLEAR   = (1 << COM1B1),           // OC1B will clear.
            OC1B_SET     = (1 << COM1B0) | (1 << COM1B1),   // OC1B will be set.
        };

        //------------------------------------------------------------------
        // Interrupts to enable.
        //------------------------------------------------------------------
        enum interrupt_t  : uint8_t {
            INT_NONE = 0,
            INT_CAPTURE = (1 << ICIE1),
            INT_COMP_MATCH_A = (1 << OCIE1A),
            INT_COMP_MATCH_B = (1 << OCIE1B),
            INT_OVERFLOW = (1 << TOIE1)
        };

        //------------------------------------------------------------------
        // FORCE COMPARE bits. These bits end up in bits FOC1A & FOC1B in 
        // the TCCR1C register.
        //------------------------------------------------------------------
        enum forceCompare_t  : uint8_t {
            FORCE_COMPARE_NONE = 0,
            FORCE_COMPARE_MATCH_A = (1 << FOC1A),
            FORCE_COMPARE_MATCH_B = (1 << FOC1B)
        };

        //------------------------------------------------------------------
        // INPUT CAPTURE NOISE CANCEL bits. These end up in bits ICNC1 & 
        // ICES1 in the TCCR1B register.
        //------------------------------------------------------------------
        enum inputCapture_t  : uint8_t {
            INPCAP_NOISE_CANCEL_OFF_FALLING_EDGE = 0,
            INPCAP_NOISE_CANCEL_OFF_RISING_EDGE = (1 << ICES1),
            INPCAP_NOISE_CANCEL_ON_FALLING_EDGE = (1 << ICNC1),
            INPCAP_NOISE_CANCEL_ON_RISING_EDGE = (1 << ICNC1) | (1 << ICES1)
        };

        //------------------------------------------------------------------
        // Initialise Timer 1 with a requested timer mode and clock source.
        // Setting  the clock source to anything other than CLK_DISABLED 
        // immediately starts the timer.
        // Optional parameters allow the definition of what happens on a
        // compare match with A and/or B which defaults to no action, the
        // interrupts to be enabled which defaults to none and whether a
        // force compare (A and/or B) is to be carried out, defaults to
        // neither.
        //------------------------------------------------------------------
        void initialise(const uint8_t timerMode, 
                        const clockSource_t clockSource, 
                        const compareMatch_t compareMatch = OC1X_DISCONNECTED, 
                        const interrupt_t enableInterrupts = INT_NONE,
                        const forceCompare_t forceCompare = FORCE_COMPARE_NONE,
                        const inputCapture_t inputCapture = INPCAP_NOISE_CANCEL_OFF_FALLING_EDGE) {
                            
            // Mode 13 is reserved, MODE_FAST_PWM_OCR1A is the upper limit.
            if (timerMode > MODE_FAST_PWM_OCR1A || timerMode == MODE_RESERVED_13) {
                return;
            }

            // Can't use OC1B_TOGGLE or FORCE COMPARE in anything but NORMAL and CTC modes.
            if ((timerMode != MODE_NORMAL && timerMode != MODE_CTC_OCR1A && timerMode != MODE_CTC_ICR1) && 
                (compareMatch == OC1B_TOGGLE || (forceCompare & FORCE_COMPARE_MATCH_A) || (forceCompare & FORCE_COMPARE_MATCH_B))) {
                return;
            }

            //------------------------------------------------------------------
            // Explicitly setting these will overwrite any previous settings
            // from the Arduino init() function, for example.
            //------------------------------------------------------------------
            TCCR1A = (timerModes[timerMode][0]) | compareMatch;
            TCCR1B = (timerModes[timerMode][1]) | clockSource | inputCapture;
            TCCR1C = forceCompare;
            TIMSK1 = enableInterrupts;
        }
      
    }  // End of Timer1 namespace  
  
}  // End of AVRAssist namespace.

#endif // __TIMER1_H__

