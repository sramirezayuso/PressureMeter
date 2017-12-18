#include <avr/io.h>
#include <inttypes.h>

void USARTInit(unsigned int ubrr)
{
   /*Set baud rate */
   UBRR0H = (unsigned char)(ubrr>>8);
   UBRR0L = (unsigned char)ubrr;
   /* Enable receiver and transmitter */
   UCSR0B = (1<<RXEN0)|(1<<TXEN0);
   /* Set frame format: 8data, 2stop bit, no parity */
   UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

unsigned char USARTReceive(void)
{
   /* Wait for data to be received */
   while ( !(UCSR0A & (1<<RXC0)) )
   ;
   /* Get and return received data from buffer */
   return UDR0;
}

void USARTTransmit(unsigned char data)
{
   /* Wait for empty transmit buffer */
   while ( !( UCSR0A & (1<<UDRE0)) )
   ;
   /* Put data into buffer, sends the data */
   UDR0 = data;
}

void USARTSendString(char* string)
{
   int i;
   for (i = 0; string[i] != 0; ++i)
   {
      USARTTransmit(string[i]);
   }
}