#include <avr/io.h>

void configureADC() {

   ADCSRA = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
   // ADEN: Set to turn on ADC , by default it is turned off 
   //ADPS2: ADPS2 and ADPS0 set to make division factor 32 
   ADMUX=0x05 | (1<<ADLAR); // ADC input channel set to PA5
}

unsigned long readADC() {

	ADCH = 0x00; // Clear the previous result

    ADCSRA |= (1<<ADSC); // Start conversion
    while (ADCSRA & (1<<ADSC)); // wait for conversion to complete

	unsigned long temp = ADC;

	return temp;

}