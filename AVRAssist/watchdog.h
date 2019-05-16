#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

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

#include <avr/wdt.h>


namespace AVRAssist {
    
    //----------------------------------------------------------------------
    // Watchdog timer setup.
    // The WDT control register is as follows:
    //
    // +-----+------+----------------------------+
    // | Bit | Name | Comments                   |
    // +-----+------+----------------------------+
    // | 7   | WDIF | Watchdog Interrupt Flag    |
    // | 6   | WDIE | Watchdog Interrupt Enable  |
    // | 5   | WDP3 | Watchdog Timer Prescaler 3 |
    // | 4   | WDCE | Watchdog Change Enable     |
    // | 3   | WDE  | Watchdog Reset Enable      |
    // | 2   | WDP2 | Watchdog Timer Prescaler 2 |
    // | 1   | WDP1 | Watchdog Timer Prescaler 1 |
    // | 0   | WDP0 | Watchdog Timer Prescaler 0 |
    // +-----+------+----------------------------+
    //
    //----------------------------------------------------------------------
    namespace Watchdog {
        
        //------------------------------------------------------------------
        // What timeout is required on the Watchdog? We have 10 to choose
        // from.
        //------------------------------------------------------------------
        enum timeout_t : uint8_t {
            WDT_TIMEOUT_16MS = 0,
            WDT_TIMEOUT_32MS,
            WDT_TIMEOUT_64MS,
            WDT_TIMEOUT_125MS,
            WDT_TIMEOUT_250MS,
            WDT_TIMEOUT_500MS,
            WDT_TIMEOUT_1S,
            WDT_TIMEOUT_2S,
            WDT_TIMEOUT_4S = (1 << WDP3),
            WDT_TIMEOUT_8S = WDT_TIMEOUT_4S | (1 << WDP0) 
        };

        //------------------------------------------------------------------
        // Define the watchdog mode. We can have RESET only, INTERRUPT only
        // or both together.
        //------------------------------------------------------------------
        enum mode_t  : uint8_t {
            WDT_MODE_RESET = (1 << WDE),
            WDT_MODE_INTERRUPT = (1 << WDIE),
            WDT_MODE_BOTH = WDT_MODE_RESET | WDT_MODE_INTERRUPT
        };
        
        //------------------------------------------------------------------
        // Initialise the watchdog with a timeout and a required mode.
        //------------------------------------------------------------------
        void initialise(const timeout_t timeout, 
                        const mode_t mode) {

            //--------------------------------------------------------------
            // Preserve existing state of the interrupt flag.
            //--------------------------------------------------------------
            uint8_t oldSREG = SREG;

            //--------------------------------------------------------------
            // Initial enabling of the watchdog:
            // 1. Disable interrupts;
            // 2. Reset the watchdog;
            // 3. Clear bit WDRF in MCUSR.
            //--------------------------------------------------------------
            cli();
            wdt_reset();
            MCUSR &= ~(1 << WDRF);
            
            //--------------------------------------------------------------
            // Next, the dreaded timed sequence to set it all up.
            // 4. Set WDCE and WDE together;
            // 5. Within 4 clocks, set the mode, with WDE now cleared;
            // 6. Re-enable interrupts, if they were originally on.
            //--------------------------------------------------------------
            WDTCSR |= ((1 << WDCE) | (1 << WDE));
            WDTCSR = (timeout | mode);
            SREG = oldSREG;
        }

    } // End of watchdog namespace.
  
}  // End of AVRAssist namespace.
#endif // __WATCHDOG_H__