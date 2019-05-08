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


//----------------------------------------------------------------------
// Timer 2 setup
//----------------------------------------------------------------------
namespace Timer2 {
    
    //------------------------------------------------------------------
    // Timer 2 Clock Sources. 
    //------------------------------------------------------------------
    enum {
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
    // What happens when OCR2A, OCR2B match TCNT2? 
    // Pin OC2A = Physical pin 12, Arduino pin D6 or AVR pin PD6.
    // Pin OC2B = Physical pin 11, Arduino pin D5 or AVR pin PD5.
    //------------------------------------------------------------------
    enum {
        DISCONNECTED = 0,                       // Nothing - OC2A, OC2B both disconnected.
        OC2A_TOGGLE  = (1 << COM0A0),           // OC2A will toggle.
        OC2A_CLEAR   = (1 << COM0A1),           // OC2A will clear.
        OC2A_SET     = (1 << COM0A0) | (1 << COM0A1),   // OC2A will be set.
        OC2B_TOGGLE  = (1 << COM0B0),           // OC2B will toggle.
        OC2B_CLEAR   = (1 << COM0B1),           // OC2B will clear.
        OC2B_SET     = (1 << COM0B0) | (1 << COM0B1),   // OC2B will be set.
    };

    //------------------------------------------------------------------
    // Timer modes. Modes 4 and 6 are reserved and not used.
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
    // Interrupts to enable.
    //------------------------------------------------------------------
    enum {
        INT_COMP_MATCH_A = (1 << OCIE2A),
        INT_COMP_MATCH_B = (1 << OCIE2B),
        INT_OVERFLOW = (1 << TOIE2)
    };

    //------------------------------------------------------------------
    // TCCR2A and TCCR2B initial settings for the various timer modes. 
    // TCCR2A is index [n][0]
    // TCCR2B is index [n][1]
    //------------------------------------------------------------------
    const byte timerModes[8][2] = {             // TCCR2A, TCCR2B
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
    // Initialise Timer 2 with a requested clock source, timer mode and 
    // output waveform, plus, if required, interrupts enabled. The 
    // latter default to all off. (No interrupts enabled.)
    //------------------------------------------------------------------
    void initialise(const uint8_t timerMode, const uint8_t clockSource, 
                    const uint8_t waveForm, const uint8_t enableInterrupts = 0) {
                        
        // Modes 4 and 6 are reserved. 
        if (timerMode < MODE_NORMAL || timerMode > MODE_FAST_PWM_OCR2A || 
            timerMode == MODE_RESERVED_4 || timerMode == MODE_RESERVED_6) {
            return;
        }

        // Can't use OC2B_TOGGLE in anything but NORMAL and CTC modes.
        if (timerMode != MODE_NORMAL && timerMode != MODE_CTC_OCR2A && waveForm == OC2B_TOGGLE) {
            return;
        }

        //------------------------------------------------------------------
        // Explicitly setting these will overwrite any previous settings
        // from the Arduino init() function, for example.
        //------------------------------------------------------------------
        TCCR2A = (timerModes[timerMode][0]) | waveForm;  
        TCCR2B = (timerModes[timerMode][1]) | clockSource;
        TIMSK2 = enableInterrupts;
    }
  
}  // End of Timer2 namespace.

#endif // __TIMER0_H__

