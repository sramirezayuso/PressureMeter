#include <avr/io.h>

#define LED_PORT PORTD
#define LED_ADDR DDRD

#define ON 0x00
#define OFF 0xFF

void prepareLeds() {
	LED_ADDR = 0XFF;
}

void turnLedOn(int i) {
	int x = 0xFE;
	int k;
	for(k = 0; k < i; k++) {
		x = x << 1;
		x = x | 0x01;
	}
	LED_PORT = LED_PORT & x;
}

void turnLedOff(int i) {
	int x = 0x01;
	x = x << i;
	LED_PORT = LED_PORT | x;
}

void toggleLed(int i) {
	int x = 0x01;
	x = x << i;
	LED_PORT = LED_PORT ^ x;
}

void turnLedsOn(int num) {
	LED_PORT =  num;
}

void turnAllOn() {
	LED_PORT = ON;
}

void turnAllOff() {
	LED_PORT = OFF;
}