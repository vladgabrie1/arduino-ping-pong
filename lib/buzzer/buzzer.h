#include <avr/io.h>
#include <usart.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

void enableBuzzer();
void playTone( float frequency, uint32_t duration );