# AVRAssist
_AVRAssist_ is a set of header files which can be `#include`d in your AVR C++ code, or, `#include`d in an Arduino Sketch where you want to get down and dirty in the various hardware bits of the AVR micro controller. The _AVRAssist_ headers make it easier to make setting various AVR registers a tad easier. 

# Components
The following AVR internal devices can be set up with the current version of _AVRAssist_:

* Timer/counters - all three timer/counters have separate header files;
* Analogue to Digital Converter;
* The Analogue Comparator;
* The Watchdog Timer.


# Example

As an example, the following will configure Timer/Counter 0 with:

* Fast PWM mode with `TOP` = 255;
* A prescaler of 64;
* `OCOA` & `OCOB` = Pins 11 & 12 = Arduino `D5` & `D6` = AVR `PD5` & `PD6` in normal GPIO mode;
* Interrupt to fire on compare match A;
* Interrupt to fire on compare match B;

````
#include <timer0.h>

using namespace AVRAssist;

...

Timer0::initialise(Timer0::MODE_FAST_PWM_255,           // Timer mode;
                   Timer0::CLK_PRESCALE_64,             // Prescaler;
                   Timer0::OCOX_DISCONNECTED,           // OCOA, OCOB actions on compare match;
                   Timer0::INT_COMP_MATCH_A |           // Interrupt(s) to enable;
                        Timer0::INT_COMP_MATCH_B
                  );
...
````

Only the first two parameters are actually required though, the rest have _sensible_ defaults. (For certain values of sensible perhaps?)

Later on in the code, you can force a comparison between TCNT0 and either OCR0A and/or OCR0B, should you have the need. This will _not_ fire any configured interrupts but will set OC0A or OC0B to the state they would take without a forced compare if they happen to match TCNT0 when forced. This is simply done as follows:

````
Timer0::forceCompare(Timer0::FORCE_COMPARE_MATCH_A |
                          Timer0::FORCE_COMPARE_MATCH_B
                    );
````

Using _AVRAssist_ is, hopefully, much easier and less prone to _fat fingered typist_ syndrome (something I suffer from, frequently!) especially when attempting to type the following, equivalent code:

````
TCCR0A = (0 << COM0A1) | (0 << COM0A0) | (0 << COM0B1) | (0 << COM0B0) | (1 << WGM00) | (1 << WGM01);
TCCR0B = (0 << FOC0A)  | (0 << FOC0B)  | (0 << WGM02)  | (0 << CS02)   | (1 << CS01)  | (1 << CS00);
TIMSK0 = (1 << OCIE0A) | (1 << OCIE0B) | (0 << TOIE0);
````

_especially_ if there's a different waveform to be applied to `TCCR0A`! At least, I find it easier! And, _yes_ I did type the above incorrectly when writing this readme. Sigh!

__NOTE:__ _Yes, I know_, I've set all the zero bits in the above equivalent code, but that makes it easier to change the bits later, if I needed to change the mode or prescaler etc.
