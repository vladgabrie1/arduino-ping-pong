#include <util/delay.h>
#include <avr/io.h>
#include<avr/sfr_defs.h>
#include<usart.h>

void enableButton( int button );
int buttonPushed( int button );
int buttonReleased( int button );