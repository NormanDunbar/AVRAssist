#ifndef __TIMER2_H__
#define __TIMER2_H__

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
    // Timer 2 setup
    //----------------------------------------------------------------------
    namespace Timer2 {
        
        //------------------------------------------------------------------
        // TIMER MODES. Modes 4 and 6 are reserved and not used. The rest 
        // are simply used to index into the timerModes[][] array below.
        //------------------------------------------------------------------
        enum {
            MODE_NORMAL = 0,                        // Mode 0: Normal mode.
            MODE_PC_PWM_255,                        // Mode 1: Phase Correct PWM, TOP = 255
            MODE_CTC_OCR2A,                         // Mode 2: CTC, TOP = OCR2A.
            MODE_FAST_PWM_255,                      // Mode 3: Fast PWM, TOP = 255.
            MODE_RESERVED_4,                        // Mode 4: Reserved, do not use.
            MODE_PC_PWM_OCR2A,                      // Mode 5: Phase Correct PWM, TOP = OCR2A.
            MODE_RESERVED_6,                        // Mode 6: Reserved, do not use.
            MODE_FAST_PWM_OCR2A                     // Mode 7: Fast PWM, TOP = ORC2A.
        };
        
        //------------------------------------------------------------------
        // TCCR2A and TCCR2B initial settings for the various timer modes. 
        // TCCR2A is index [n][0]
        // TCCR2B is index [n][1]
        // These bits end up in WGM21 & WGM20 in TCCR2A and WGM22 in TCCR2B
        // depending on the timer mode chosen from the above.
        //------------------------------------------------------------------
        const uint8_t timerModes[8][2] = {          // [n][0] = TCCR2A, [n][1] = TCCR2B
            {0, 0},                                 // Normal, TOP = 255
            {(1 << WGM20), 0},                      // Phase Correct PWM, TOP = 255
            {(1 << WGM21), 0},                      // Clear Timer on Compare, TOP = OCR2A
            {((1 << WGM20) | (1 << WGM21)), 0},     // Fast PWM, TOP = 255
            {0, (1 << WGM22)},                      // Reserved - don't use
            {((1 << WGM20), (1 << WGM22))},         // Phase Correct PWM, TOP = OCR2A
            {((1 << WGM21), (1 << WGM22))},         // Reserved - don't use
            {((1 << WGM20) | (1 << WGM21), (1 << WGM22))}     // Fast PWM, TOP = OCR2A
        };

        //------------------------------------------------------------------
        // CLOCK SOURCES. 
        // These bits end up in CS22, CS21, CS20 in register TCCR2B.
        //------------------------------------------------------------------
        enum clockSource_t {
            CLK_DISABLED,                           // No clock running
            CLK_PRESCALE_1,                         // Prescaler = divide by 1
            CLK_PRESCALE_8,                         // Prescaler = divide by 8
            CLK_PRESCALE_32,                        // Prescaler = divide by 32
            CLK_PRESCALE_64,                        // Prescaler = divide by 64
            CLK_PRESCALE_128,                       // Prescaler = divide by 128
            CLK_PRESCALE_256,                       // Prescaler = divide by 256
            CLK_PRESCALE_1024                       // Prescaler = divide by 1024
        };

        //------------------------------------------------------------------
        // COMPARE MATCH bits.
        // What happens when OCR2A, OCR2B match TCNT2? 
        // Pin OC2A = Physical pin 12, Arduino pin D6 or AVR pin PD6.
        // Pin OC2B = Physical pin 11, Arduino pin D5 or AVR pin PD5.
        //------------------------------------------------------------------
        enum compareMatch_t {
            OC2X_DISCONNECTED = 0,                  // Nothing - OC2A, OC2B both disconnected.
            OC2A_TOGGLE  = (1 << COM0A0),           // OC2A will toggle.
            OC2A_CLEAR   = (1 << COM0A1),           // OC2A will clear.
            OC2A_SET     = (1 << COM0A0) | (1 << COM0A1),   // OC2A will be set.
            OC2B_TOGGLE  = (1 << COM0B0),           // OC2B will toggle.
            OC2B_CLEAR   = (1 << COM0B1),           // OC2B will clear.
            OC2B_SET     = (1 << COM0B0) | (1 << COM0B1),   // OC2B will be set.
        };

        //------------------------------------------------------------------
        // INTERRUPTS to enable. These bits end up in the TIMSK2 register.
        //------------------------------------------------------------------
        enum interrupt_t {
            INT_NONE = 0,
            INT_COMPARE_MATCH_A = (1 << OCIE2A),
            INT_COMPARE_MATCH_B = (1 << OCIE2B),
            INT_OVERFLOW = (1 << TOIE2)
        };

        //------------------------------------------------------------------
        // FORCE COMPARE bits. These bits end up in bits FOC2A & FOC2B in 
        // the TCCR2B register.
        //------------------------------------------------------------------
        enum forceCompare_t {
            FORCE_COMPARE_NONE = 0,
            FORCE_COMPARE_MATCH_A = (1 << FOC2A),
            FORCE_COMPARE_MATCH_B = (1 << FOC2B)
        };
        
        //------------------------------------------------------------------
        // Initialise Timer 2 with a requested timer mode and clock source.
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
                        const compareMatch_t compareMatch = OC2X_DISCONNECTED, 
                        const interrupt_t enableInterrupts = INT_NONE,
                        const forceCompare_t forceCompare = FORCE_COMPARE_NONE) {
                            
            // Modes 4 and 6 are reserved, MODE_FAST_PWM_OCR2A is the upper limit. 
            if (timerMode > MODE_FAST_PWM_OCR2A || timerMode == MODE_RESERVED_4 || timerMode == MODE_RESERVED_6) {
                return;
            }

            // Can't use OC0B_TOGGLE or FORCE COMPARE  in anything but NORMAL and CTC modes.
            if ((timerMode != MODE_NORMAL && timerMode != MODE_CTC_OCR2A) && 
                (compareMatch == OC2B_TOGGLE || (forceCompare & FORCE_COMPARE_MATCH_A) || (forceCompare & FORCE_COMPARE_MATCH_B)) {
                return;
            }

            //------------------------------------------------------------------
            // Explicitly setting these will overwrite any previous settings
            // from the Arduino init() function, for example.
            //------------------------------------------------------------------
            TCCR2A = (timerModes[timerMode][0]) | compareMatch;
            TCCR2B = (timerModes[timerMode][1]) | clockSource | forceCompare;
            TIMSK2 = enableInterrupts;
        }
      
    }  // End of Timer2 namespace.
  
}  // End of AVRAssist namespace.

#endif // __TIMER0_H__

