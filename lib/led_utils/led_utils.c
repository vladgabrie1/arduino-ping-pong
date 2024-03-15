#include "led_utils.h"
#include <avr/io.h>

#define NUMBER_OF_LEDS 4

void enableLed(int lednumber) {
    if (lednumber < 0 || lednumber > NUMBER_OF_LEDS - 1) return;
    DDRB |= (1 << (PB2 + lednumber));
}

void lightUpLed(int lednumber) {
    if (lednumber < 0 || lednumber > NUMBER_OF_LEDS - 1) return;
    PORTB &= ~(1 << (PB2 + lednumber));
}

void lightDownLed(int lednumber) {
    if (lednumber < 0 || lednumber > NUMBER_OF_LEDS - 1) return;
    PORTB |= (1 << (PB2 + lednumber));
}

void enableMultipleLeds(uint8_t leds) {
    if (leds < 0 || leds > 60) return;
    DDRB = leds;
}

void lightUpMultipleLeds(uint8_t leds) {
    if (leds < 0 || leds > 60) return;
    PORTB &= ~leds;
}

void enableAllLeds(){
    enableLed(0);
    enableLed(1);
    enableLed(2);
    enableLed(3);
}

void lightDownMultipleLeds(uint8_t leds) {
    if (leds < 0 || leds > 60) return;
    PORTB |= leds;
}

void lightUpAllLeds() {
    PORTB &= ~0b00111100;
}

void lightDownAllLeds() {
    PORTB |= 0b00111100;
}

void lightToggleOneLed(int led){
    if (led < 0 || led > NUMBER_OF_LEDS - 1) return;
    PORTB ^= (1 << (PB2 + led));
}
