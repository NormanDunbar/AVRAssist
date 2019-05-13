#ifndef __TIMER0_H__
#define __TIMER0_H__

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
    // Timer 0 setup
    //----------------------------------------------------------------------
    namespace Timer0 {
        
        //------------------------------------------------------------------
        // TIMER MODES. Modes 4 and 6 are reserved and not used. The rest
        // are simply used to index into the timerModes[][] array below.
        //------------------------------------------------------------------
        enum  : uint8_t {
            MODE_NORMAL = 0,                        // Mode 0: Normal mode.
            MODE_PC_PWM_255,                        // Mode 1: Phase Correct PWM, TOP = 255
            MODE_CTC_OCR0A,                         // Mode 2: CTC, TOP = OCR0A.
            MODE_FAST_PWM_255,                      // Mode 3: Fast PWM, TOP = 255.
            MODE_RESERVED_4,                        // Mode 4: Reserved, do not use.
            MODE_PC_PWM_OCR0A,                      // Mode 5: Phase Correct PWM, TOP = OCR0A.
            MODE_RESERVED_6,                        // Mode 6: Reserved, do not use.
            MODE_FAST_PWM_OCR0A                     // Mode 7: Fast PWM, TOP = ORC0A.
        };

        //------------------------------------------------------------------
        // TCCR0A and TCCR0B initial settings for the various timer modes. 
        // TCCR0A is index [n][0]
        // TCCR0B is index [n][1]
        // These bits end up in WGM01 & WGM00 in TCCR0A and WGM02 in TCCR0B
        // depending on the timer mode chosen from the above.
        //------------------------------------------------------------------
        const uint8_t timerModes[8][2] = {          // [n][0] = TCCR0A, [n][1] = TCCR0B
            {0, 0},                                 // Normal, TOP = 255
            {(1 << WGM00), 0},                      // Phase Correct PWM, TOP = 255
            {(1 << WGM01), 0},                      // Clear Timer on Compare, TOP = OCR0A
            {((1 << WGM00) | (1 << WGM01)), 0},     // Fast PWM, TOP = 255
            {0, (1 << WGM02)},                      // Reserved - don't use
            {((1 << WGM00), (1 << WGM02))},         // PWM - Phase-correct, TOP = OCR0A
            {((1 << WGM01), (1 << WGM02))},         // Reserved - don't use
            {((1 << WGM00) | (1 << WGM01), (1 << WGM02))}     // Fast PWM, TOP = OCR0A
        };

        //------------------------------------------------------------------
        // CLOCK SOURCES. Note that external source, pin 'T0' is  physical
        // pin 6, Arduino pin D4 or AVR pin PD4.
        // These bits end up in CS02, CS01, CS00 in register TCCR0B.
        //------------------------------------------------------------------
        enum clockSource_t : uint8_t {
            CLK_DISABLED,                           // No clock running
            CLK_PRESCALE_1,                         // Prescaler = divide by 1
            CLK_PRESCALE_8,                         // Prescaler = divide by 8
            CLK_PRESCALE_64,                        // Prescaler = divide by 64
            CLK_PRESCALE_256,                       // Prescaler = divide by 256
            CLK_PRESCALE_1024,                      // Prescaler = divide by 1024
            CLK_T0_FALLING,                         // External pin T0, falling edge
            CLK_T0_RISING                           // External pin T0, rising edge
        };

        //------------------------------------------------------------------
        // COMPARE MATCH bits.
        // What happens when OC0RA, OCR0B match TCNT0? 
        // Pin OC0A = Physical pin 12, Arduino pin D6 or AVR pin PD6.
        // Pin OC0B = Physical pin 11, Arduino pin D5 or AVR pin PD5.
        // These bits are destined for COM0A1, COM0A0, COM0B1 and COM0B0 in
        // register TCCR0A.
        //------------------------------------------------------------------
        enum compareMatch_t  : uint8_t {
            OCOX_DISCONNECTED = 0,                  // Nothing, OC0A, OC0B both disconnected.
            OCOA_TOGGLE  = (1 << COM0A0),           // OC0A will toggle.
            OCOA_CLEAR   = (1 << COM0A1),           // OC0A will clear.
            OCOA_SET     = (1 << COM0A0) | (1 << COM0A1),   // OC0A will be set.
            OCOB_TOGGLE  = (1 << COM0B0),           // OC0B will toggle.
            OCOB_CLEAR   = (1 << COM0B1),           // OC0B will clear.
            OCOB_SET     = (1 << COM0B0) | (1 << COM0B1),   // OC0B will be set.
        };

        //------------------------------------------------------------------
        // INTERRUPTS to enable. These bits end up in the TIMSK0 register.
        //------------------------------------------------------------------
        enum interrupt_t  : uint8_t {
            INT_NONE = 0,
            INT_COMPARE_MATCH_A = (1 << OCIE0A),
            INT_COMPARE_MATCH_B = (1 << OCIE0B),
            INT_OVERFLOW = (1 << TOIE0)
        };

        //------------------------------------------------------------------
        // FORCE COMPARE bits. These bits end up in bits FOC0A & FOC0B in 
        // the TCCR0B register.
        //------------------------------------------------------------------
        enum forceCompare_t  : uint8_t {
            FORCE_COMPARE_NONE = 0,
            FORCE_COMPARE_MATCH_A = (1 << FOC0A),
            FORCE_COMPARE_MATCH_B = (1 << FOC0B)
        };


        //------------------------------------------------------------------
        // Initialise Timer 0 with a requested timer mode and clock source.
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
                        const compareMatch_t compareMatch = OCOX_DISCONNECTED, 
                        const interrupt_t enableInterrupts = INT_NONE
                        const forceCompare_t forceCompare = FORCE_COMPARE_NONE) {
                            
            // Modes 4 and 6 are reserved, MODE_FAST_PWM_OCR0A is the upper limit.
            if (timerMode > MODE_FAST_PWM_OCR0A || timerMode == MODE_RESERVED_4 || timerMode == MODE_RESERVED_6) {
                return;
            }

            // Can't use OC0B_TOGGLE or FORCE COMPARE  in anything but NORMAL and CTC modes.
            if ((timerMode != MODE_NORMAL && timerMode != MODE_CTC_OCR0A) && 
                (compareMatch == OC0B_TOGGLE || (forceCompare & FORCE_COMPARE_MATCH_A) || (forceCompare & FORCE_COMPARE_MATCH_B)) {
                return;
            }

            //------------------------------------------------------------------
            // Explicitly setting these will overwrite any previous settings
            // from the Arduino init() function, for example.
            //------------------------------------------------------------------
            TCCR0A = (timerModes[timerMode][0]) | compareMatch;
            TCCR0B = (timerModes[timerMode][1]) | clockSource | forceCompare;
            TIMSK0 = enableInterrupts;
        }
      
    }  // End of Timer0 namespace.
  
}  // End of AVRAssist namespace.

#endif // __TIMER0_H__

