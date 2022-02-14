/*
 * UltraSonic Sensor Lab.c
 *
 * Created: 4/3/2021 10:42:38 AM
 * Author : ashto
 */ 

#define F_CPU 16000000UL
#define ECHO1 0 //PB0
#define TRIGGER1 1 //PB1

#define TRIGGER2 3 //PB3
#define LED 2 //PB2

double preLoad; // for led
void blinker(double);

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

void usart_init(void);
void usart_send(unsigned char);
void send_string(char *stringAddress);
float ultra_sonic_sensor1(unsigned int timeToRisingEdge, unsigned int timeToFallingEdge, unsigned int pulseWidth, float distance1);
float ultra_sonic_sensor2(unsigned int timeToRisingEdge, unsigned int timeToFallingEdge, unsigned int pulseWidth, float distance2);

int main(void)
{
	char buffer1[10];
	char buffer2[10];
	
	// timer
	sei();
	uint16_t echorising = 0;
	uint16_t echofalling = 0;
	uint16_t usstime = 0;
	TCCR1B = (1<<CS12) | (1<<CS10); // 1024 
	TCNT1 = 0;

	// ------ Ultra Sonic Sensor ------
	unsigned int timeToRisingEdge, timeToFallingEdge, pulseWidth;
	float distance1, distance2;
	usart_init();
	TCCR1A = 0; // timer mode - normal
	
	DDRB = (1<<TRIGGER1) | (1<<TRIGGER2); // led and trigger outputs
	
	
    /* Replace with your application code */
    while (1) 
    {
		// ------ Ultra Sonic Sensor 1 ------
		//distance1 = ultra_sonic_sensor1(timeToRisingEdge, timeToFallingEdge, pulseWidth, distance1);
		
		_delay_ms(500);
		
		// Trigger Pulse
		PORTB |= 1<< TRIGGER1;
		_delay_us(10);
		PORTB &= ~(1<< TRIGGER1);
		
		// Grab time value
		echorising = TCNT1;
		// Wait for echo high
		while ((PINB & (1<<PINB0))  ==  1);
		// Grab time value
		echofalling = TCNT1;

		usstime = echofalling - echorising;
		
		
		
		
		
		// ------ Ultra Sonic Sensor 2 ------
		//distance2 = ultra_sonic_sensor2(timeToRisingEdge, timeToFallingEdge, pulseWidth, distance2);
		_delay_ms(500);
		
		// ------ Printing ------		
		
		dtostrf(echorising, 5, 1, buffer1); // convert distance to a string
		dtostrf(echofalling, 5, 1, buffer2); // convert distance to a string
		send_string(buffer1); // transmit distance in cm
		usart_send(' ');
		send_string(buffer2);
		usart_send(' ');
		usart_send(13); // tx carraige return
		usart_send(10); // tx new line
		
    }
}

float ultra_sonic_sensor1(unsigned int timeToRisingEdge, unsigned int timeToFallingEdge, unsigned int pulseWidth, float distance1){
	static volatile int pulse = 0;
	int16_t count_a = 0;
	
	// ------ Ultra Sonic Sensor 1 ------
	
	// Trig: Provide a 10.0 us pulse to ultra sonic sensor
	PORTB |= 1<< TRIGGER1; 
	_delay_us(10);
	PORTB &= ~(1<< TRIGGER1); 
	
	
	count_a = pulse/58;
		
	// Echo: Measure duration of pulse
	TCCR1B |= (1<<ICES1); // start counting until rising edge, prescalar 1023
	while((TIFR1 & (1 << ICF1)) == 0); // wait for rising edge on ICP1/PB0
	timeToRisingEdge = ICR1; // save the time for rising edge
	TIFR1 = (1 << ICF1); // clear ICF1 flag
	
	TCCR1B &= ~(1<<ICES1); // start counting until falling edge, prescalar 1023
	while ((TIFR1 & (1<< ICF1)) == 0); // wait for falling edge ICP1/PB0
	timeToFallingEdge = ICR1; // save falling edge delay
	TIFR1 = (1 << ICF1); //clear ICF1 flag
	
	pulseWidth = timeToFallingEdge - timeToRisingEdge; // delay between edges
	distance1 = pulseWidth * 1.098; // one way distance to target in cm, * 1000 for the hundreds place
	
	return distance1;
}

float ultra_sonic_sensor2(unsigned int timeToRisingEdge, unsigned int timeToFallingEdge, unsigned int pulseWidth, float distance2){
	// ------ Ultra Sonic Sensor 2 ------
	
	// Trig: Provide a 10.0 us pulse to ultra sonic sensor
	PORTB |= 1<< TRIGGER2;
	_delay_us(10);
	PORTB &= ~(1<< TRIGGER2);
	
	// Echo: Measure duration of pulse
	TCCR1B |= (1<<ICES1); // start counting until rising edge, prescalar 1023
	while((TIFR1 & (1 << ICF1)) == 0); // wait for rising edge on ICP1/PB0
	timeToRisingEdge = ICR1; // save the time for rising edge
	TIFR1 = (1 << ICF1); // clear ICF1 flag
	
	TCCR1B &= ~(1<<ICES1); // start counting until falling edge, prescalar 1023
	while ((TIFR1 & (1<< ICF1)) == 0); // wait for falling edge ICP1/PB0
	timeToFallingEdge = ICR1; // save falling edge delay
	TIFR1 = (1 << ICF1); //clear ICF1 flag
	
	pulseWidth = timeToFallingEdge - timeToRisingEdge; // delay between edges
	distance2 = pulseWidth * 1.098; // one way distance to target in cm, * 1000 for the hundreds place
	
	return distance2;
}

void usart_init(void){
	UCSR0B = (1<< TXEN0); // enable USART transmitter
	UCSR0C = (1<< UCSZ01) | (1 << UCSZ00); // set 8 bit character size
	UBRR0L = 103; // baud rate to 9600
}

void usart_send(unsigned char ch){
	while (!(UCSR0A & (1 << UDRE0))); // wait until tx data buffer empty
	UDR0 = ch; //write the character to the USART data register
}

void send_string(char *stringAddress){
	unsigned char i;
	for (i = 0; i < strlen(stringAddress); i++)
		usart_send(stringAddress[i]);
}

void blinker(double frequency){
	preLoad = 65536 - 7812 / frequency;
}

ISR(TIMER1_OVF_vect){
	TCNT1 = preLoad;
	PORTB ^= (1<<LED);
}

