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


//----------------------------------------------------------------------
// Timer 0 setup
//----------------------------------------------------------------------
namespace Timer0 {
    
    //------------------------------------------------------------------
    // Timer 0 Clock Sources. Note that external source, pin 'T0' is 
    // physical pin 6, Arduino pin D4 or AVR pin PD4.
    //------------------------------------------------------------------
    enum {
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
    // What happens when OC0RA, OCR0B match TCNT0? 
    // Pin OC0A = Physical pin 12, Arduino pin D6 or AVR pin PD6.
    // Pin OC0B = Physical pin 11, Arduino pin D5 or AVR pin PD5.
    //------------------------------------------------------------------
    enum {
        DISCONNECTED = 0,                       // Nothing - OC0A, OC0B both disconnected.
        OC0A_TOGGLE  = (1 << COM0A0),           // OC0A will toggle.
        OC0A_CLEAR   = (1 << COM0A1),           // OC0A will clear.
        OC0A_SET     = (1 << COM0A0) | (1 << COM0A1),   // OC0A will be set.
        OC0B_TOGGLE  = (1 << COM0B0),           // OC0B will toggle.
        OC0B_CLEAR   = (1 << COM0B1),           // OC0B will clear.
        OC0B_SET     = (1 << COM0B0) | (1 << COM0B1),   // OC0B will be set.
    };

    //------------------------------------------------------------------
    // Timer modes. Modes 4 and 6 are reserved and not used.
    //------------------------------------------------------------------
    enum {
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
    // Interrupts to enable.
    //------------------------------------------------------------------
    enum {
        INT_COMP_MATCH_A = (1 << OCIE0A),
        INT_COMP_MATCH_B = (1 << OCIE0B),
        INT_OVERFLOW = (1 << TOIE0)
    };

    //------------------------------------------------------------------
    // TCCR0A and TCCR0B initial settings for the various timer modes. 
    // TCCR0A is index [n][0]
    // TCCR0B is index [n][1]
    //------------------------------------------------------------------
    const byte timerModes[8][2] = {             // TCCR0A, TCCR0B
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
    // Initialise Timer 0 with a requested clock source, timer mode and 
    // output waveform, plus, if required, interrupts enabled. The 
    // latter default to all off. (No interrupts enabled.)
    //------------------------------------------------------------------
    void initialise(const uint8_t timerMode, const uint8_t clockSource, 
                    const uint8_t waveForm, const uint8_t enableInterrupts = 0) {
                        
        // Modes 4 and 6 are reserved. 
        if (timerMode < MODE_NORMAL || timerMode > MODE_FAST_PWM_OCR20 || 
            timerMode == MODE_RESERVED_4 || timerMode == MODE_RESERVED_6) {
            return;
        }

        // Can't use OC0B_TOGGLE in anything but NORMAL and CTC modes.
        if (timerMode != MODE_NORMAL && timerMode != MODE_CTC_OCR0A && waveForm == OC0B_TOGGLE) {
            return;
        }

        //------------------------------------------------------------------
        // Explicitly setting these will overwrite any previous settings
        // from the Arduino init() function, for example.
        //------------------------------------------------------------------
        TCCR0A = (timerModes[timerMode][0]) | waveForm;  
        TCCR0B = (timerModes[timerMode][1]) | clockSource;
        TIMSK0 = enableInterrupts;
    }
  
}  // End of Timer0 namespace.

#endif // __TIMER0_H__

