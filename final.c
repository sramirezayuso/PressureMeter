#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#define MINS_ADDR 50
#define HOURS_ADDR 100

#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "adc.c"
#include "leds.c"
#include "usart.c"

char buffer[50];
long secs;

unsigned int duration = 0;
long target = 0;
long error = 0;

long id = 0;
unsigned int reading_left = 0;
int failed = 0;

long test_log[100][3];
int idx = 0;

void computeResult();

ISR(TIMER1_COMPA_vect) {
	secs++;
	if (reading_left > 0) {
		reading_left--;
		char message[100];
		long pressure = (long) ((readADC() - 2176) / 3);
		if (pressure - target > error || target - pressure > error) {
			failed = 1;
		}
		sprintf(message, "\r\nRead value %ld, expected %ld with error %ld", pressure, target, error);
		USARTSendString(message);
		if (reading_left == 0) {
			computeResult();
		}
	}
}

void getLog(long piece_id) {
	int i;
	long testsnum = 0;
	long failures = 0;
	for (i = 0; i < 100; i++) {
		if (test_log[i][0] == piece_id) {
			char message[100];
			testsnum++;
			if (test_log[i][2]) {
				failures++;
				sprintf(message, "[%ld] Piece %ld, Test Failed\r\n", test_log[i][1], piece_id);
			} else {
				sprintf(message, "[%ld] Piece %ld, Test Passed\r\n", test_log[i][1], piece_id);
			}
			USARTSendString(message);
		}
	}
	char message[100];
	sprintf(message, "Piece %ld, Total Tests: %ld, Failure Rate %ld\r\n", piece_id, testsnum, ((failures*100) / testsnum));
	USARTSendString(message);

}

void computeResult() {
	test_log[idx][0] = id;
	test_log[idx][1] = secs;
	test_log[idx][2] = failed;

	if (failed) {
		USARTSendString("\r\nCycle finished: Failure\r\n");
	} else {
		USARTSendString("\r\nCycle finished: Success\r\n");
	}

	failed = 0;
	id = 0;
	idx++;
	if (idx == 100) {
		idx = 0;
	}
	toggleLed(3);
}

void parseCommand() {
	if (strncmp(buffer, "get_date", 8) == 0) {
		char aux[100];
		sprintf(aux,"\r\nThe date is: %ld\r\n", secs);
		USARTSendString(aux);
	} else if (strncmp(buffer, "set_date", 8) == 0) {
		long x = strtol(&buffer[9], NULL, 0);
		if (x != 0) {
			secs = x;
			char aux[100];
			sprintf(aux, "\r\nDate set to: %ld\r\n", x);
			USARTSendString(aux);
		} else {
			USARTSendString("\r\nIncorrect date\r\n");
		}
	} else if (strncmp(buffer, "configure_cycle", 15) == 0) {
		char *ptr;
		duration = strtol(&buffer[16], &ptr, 10);
		target = strtol(ptr, &ptr, 10);
		error = strtol(ptr, &ptr, 10);

		char response[100];
		sprintf(response, "\r\nCycle set to last %d seconds with target pressure %ldpa with maximum error %ldpa\r\n", duration, target, error);
		USARTSendString(response);
	} else if (strncmp(buffer, "start_cycle", 11) == 0) {
		toggleLed(3);
		id = strtol(&buffer[12], NULL, 10);
		reading_left = duration;

		char response[100];
		sprintf(response, "\r\nStarting cycle for piece %ld\r\n", id);
		USARTSendString(response);
	} else if (strncmp(buffer, "get_log", 7) == 0) {
		long piece_id = strtol(&buffer[8], NULL, 10);

		char response[100];
		sprintf(response, "\r\nRetrieving log for piece %ld\r\n", piece_id);
		USARTSendString(response);
		getLog(piece_id);
	} else {
		USARTSendString("\r\nCommand Not Recognized\r\n");
	}
}

void initTimer() {
	TCCR1A = (1<<COM1A1);

	TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12);

	OCR1AH = 0x3D;
	OCR1AL = 0x09;

	TIMSK1 = (1<<OCIE1A);

	sei();
}

void main() {
	configureADC();

	prepareLeds();
	turnAllOff();

	initTimer();

	char data;

	USARTInit(129);

	int i = 0;
	while (1) {
		while (data != ';') {
			data = USARTReceive();
			USARTTransmit(data);
			buffer[i] = data;
			i = (i + 1) % 50;
		}
		buffer[i + 1] = '\0';
		i = 0;
		parseCommand();
		data = '\0';
	}
}