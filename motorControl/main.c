/*
 * GccApplication1.c
 *
 * Created: 8/7/2016 5:04:55 PM
 * Author : Faran Malik
 */ 

#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"


void setup(void)
{
	/* DDRx (Data Direction Register) Configures data direction of port pins 8 */ 
	DDRB = 0b00001000; // Make pin 3 of Port B an output pin 
	DDRC = 0b00000011; // Make pin 0 and pin 1 of Port C an output pin
	DDRD = 0b10000011; // Make pin 7 (motor), pin 1 (TX) of Port D, pin0 (RX) an output pin
    PORTD |= (1 << PIND0);  // activating the pull-up resistor for the RX pin (need this for sure otherwise no Serial communciation possible.... cannot send rx)
	OSCCAL = 0xAA; // Oscillator Calibration
}


void forward()
{
	PORTC = 0b00000001; // right forward
	PORTB = 0b00001000; // left forward
	PORTD = 0b00000011; // left backward clear
}

void reverse()
{
	PORTC = 0b00000110; // right backwards
	PORTB = 0b00000000; // left forward clear
	PORTD = 0b10000011; // left backwards	
}

void right()
{
	PORTB = 0b00001000; // left forward
	PORTC = 0b00000000;
	PORTD = 0b00000011;
}

void left()
{
	PORTC = 0b00000001; // right forward
	PORTB = 0b00000000; // left forward clear
	PORTD = 0b00000011;
}

void brake()
{
	PORTC = 0b00000000;
	PORTB = 0b00000000;
	PORTD = 0b00000011;
}

typedef struct keyCtrl_S
{
	char forward;
	char reverse;
	char left;
	char right;	
	char brake; 
} keyCtrl_T;

typedef struct motorCtrl_S // probably not a good idea
{
	uint8_t forward; 
	uint8_t reverse; 
	uint8_t left; 
	uint8_t right; 
	uint8_t brake; 
} motorCtrl_T;



int main(void)
{
	USARTInit(BAUD_PRESCALE); 
	char data;
    setup();
	
	// Set Keys for Control 
	keyCtrl_T keyControl; 
	keyControl.forward = 'w';
	keyControl.reverse = 's';
	keyControl.left = 'a';
	keyControl.right = 'd';
	keyControl.brake = 'x';
	
	
/* Loop Infinitely */ 
    while (1) 
    {
		//PORTC = 0b00000100; // "break point" 
		//_delay_ms(1000)
		data = USARTReadChar(); // does not go to next line unless input taken in
		
		if (data == keyControl.forward)
		{
			USART_putstring("Moving Forward...\r\n");
			forward();
		}
		else if (data == keyControl.reverse)
		{
			USART_putstring("Moving Reverse...\r\n");
			reverse();
		}
		else if (data == keyControl.right)
		{
			USART_putstring("Moving Right...\r\n");
			right(); 
		}
		else if (data == keyControl.left)
		{
			USART_putstring("Moving Left...\r\n");
			left(); 
		}
		else if (data == keyControl.brake)
		{
			USART_putstring("Stopping...\r\n");
			brake(); 
		}	
	}

		
}
