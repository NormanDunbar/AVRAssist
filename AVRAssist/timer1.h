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


//----------------------------------------------------------------------
// Timer 1 setup
//----------------------------------------------------------------------
namespace Timer1 {
    
    //------------------------------------------------------------------
    // Timer 1 Clock Sources. Note that external source, pin 'T1' is 
    // physical pin 11, Arduino pin D5 or AVR pin PD5.
    //------------------------------------------------------------------
    enum {
        CLK_DISABLED,                           // No clock running
        CLK_PRESCALE_1,                         // Prescaler = divide by 1
        CLK_PRESCALE_8,                         // Prescaler = divide by 8
        CLK_PRESCALE_64,                        // Prescaler = divide by 64
        CLK_PRESCALE_256,                       // Prescaler = divide by 256
        CLK_PRESCALE_1024                       // Prescaler = divide by 1024
        CLK_T1_FALLING,                         // External pin T0, falling edge
        CLK_T1_RISING                           // External pin T0, rising edge
    };

    //------------------------------------------------------------------
    // What happens when OCR1A, OCR1B match TCNT1? 
    // Pin OC1A = Physical pin 17, Arduino pin D11 or AVR pin PB3.
    // Pin OC1B = Physical pin 5, Arduino pin D3 or AVR pin PD3.
    //------------------------------------------------------------------
    enum {
        DISCONNECTED = 0,                       // Nothing - OC1A, OC1B both disconnected.
        OC1A_TOGGLE  = (1 << COM1A0),           // OC1A will toggle.
        OC1A_CLEAR   = (1 << COM1A1),           // OC1A will clear.
        OC1A_SET     = (1 << COM1A0) | (1 << COM1A1),   // OC1A will be set.
        OC1B_TOGGLE  = (1 << COM1B0),           // OC1B will toggle.
        OC1B_CLEAR   = (1 << COM1B1),           // OC1B will clear.
        OC1B_SET     = (1 << COM1B0) | (1 << COM1B1),   // OC1B will be set.
    };

    //------------------------------------------------------------------
    // Timer modes. Mode 13 is reserved and not used.
    //------------------------------------------------------------------
    enum {
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
        MODE_FAST_PWM_1CR1                      // Mode 14: Fast PWM, TOP = ICR1.
        MODE_FAST_PWM_OCR1A                     // Mode 15: Fast PWM, TOP = OCR1A.
    };
    
    //------------------------------------------------------------------
    // Interrupts to enable.
    //------------------------------------------------------------------
    enum {
        INT_COMP_MATCH_A = (1 << OCIE1A),
        INT_COMP_MATCH_B = (1 << OCIE1B),
        INT_OVERFLOW = (1 << TOIE1)
    };

    //------------------------------------------------------------------
    // TCCR1A and TCCR1B initial settings for the various timer modes. 
    // TCCR1A is index [n][0]
    // TCCR1B is index [n][1]
    //------------------------------------------------------------------
    const byte timerModes[16][2] = {                    // TCCR1A, TCCR1B
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
    // Initialise Timer 1 with a requested clock source, timer mode and 
    // output waveform, plus, if required, interrupts enabled. The 
    // latter default to all off. (No interrupts enabled.)
    //------------------------------------------------------------------
    void initialise(const uint8_t timerMode, const uint8_t clockSource, 
                    const uint8_t waveForm, const uint8_t enableInterrupts = 0) {
                        
        // Mode 13 is reserved. 
        if (timerMode < MODE_NORMAL || timerMode > MODE_FAST_PWM_OCR1A || 
            timerMode == MODE_RESERVED_13) {
            return;
        }

        //------------------------------------------------------------------
        // Explicitly setting these will overwrite any previous settings
        // from the Arduino init() function, for example.
        //------------------------------------------------------------------
        TCCR1A = (timerModes[timerMode][0]) | waveForm;  
        TCCR1B = (timerModes[timerMode][1]) | clockSource;
        TCCR1C = 0;
        TIMS12 = enableInterrupts;
    }
  
}  // End of Timer1 namespace  

#endif // __TIMER1_H__

