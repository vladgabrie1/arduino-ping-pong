#include <util/delay.h>
#include <avr/io.h>
#include<avr/sfr_defs.h>
#include<usart.h>
#include "display.h"
#include <ctype.h>
#include <avr/io.h>
#include <util/delay.h>


#define NUMBER_OF_BUTTONS 3
void enableButton(int button)
{
    if ( button < 0 || button > NUMBER_OF_BUTTONS-1 ) return;
    DDRC &= ~_BV( PC1+button ); /* Button 1 is connected to C1.
                             * We set the corresponding bit in the Data Direction Register to 0. */
    PORTC |= _BV( PC1+button );  /* Activate the Pull-up of C1: PINC will be set to high */
   
}

int buttonPushed(int button)
{
    if ( button < 0 || button > NUMBER_OF_BUTTONS-1 ) return -1;
   if ( bit_is_clear( PINC, (PC1+button) )) /* instead of (!(PINC & ( 1 << PC1 ))) we use the macro bit_is_clear */
        {
            return 1;
        }
    return 0;
}

int buttonReleased(int button)
{
    PORTC &= _BV( PC1+button );
    if ( button < 0 || button > NUMBER_OF_BUTTONS-1 ) return -1;
    if (!( bit_is_clear( PINC, (PC1+button) ))) /* instead of (!(PINC & ( 1 << PC1 ))) we use the macro bit_is_clear */
        {
           return 1;
        }
    return 0;
}


