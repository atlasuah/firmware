#include <avr/interrupt.h>
#include <avr/wdt.h>

void setup()
{
  cli();
  MCUSR &= ~(1<<WDRF);			// Clear WDRF if it has been unintentionally set. 
  WDTCSR = (1<<WDCE )|(1<<WDE );	// Enable configuration change. 
  WDTCSR = (1<<WDIF)|(1<<WDIE)|	        // Enable Watchdog Interrupt Mode. 
	(1<<WDCE )|(0<<WDE )|	        // Disable Watchdog System Reset Mode if unintentionally enabled. 
        //(0<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0);		// Set Watchdog Timeout period to 32 ms.
        (0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(0<<WDP0);		// Set Watchdog Timeout period to 64 ms.
        //(0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(1<<WDP0);		// Set Watchdog Timeout period to 128 ms.
  sei();
}

void loop()
{
  wdt_reset();              //Call this to reset the timer's value.
}

ISR(WDT_vect)
{
                            //The actual value reassignment happens here
