/*
 * GccApplication1.c
 *
 * Created: 8/7/2016 5:04:55 PM
 * Author : Faran Malik
 */ 

#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define buttonAvl 1

#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"
#include "control.h"
#include "pushButton.h"

void setup(void)
{
	/* DDRx (Data Direction Register) Configures data direction of port pins */ 
	DDRB |= (1<<DDB3)|(1<<DDB1); // 0b00001000 Make pin 3 of Port B an output pin. Pin 0 and pin 1 kept as inputs for buttons.  
	DDRC |= (1<<DDC0)|(1<<DDC1); // 0b00000011 Make pin 0 and pin 1 of Port C an output pin
	DDRD |= (1<<DDD1)|(1<<DDD7); // 0b10000011 Make pin 7 (motor), pin 1 (TX) of Port D, pin0 (RX) is an input pin so leave alone
    /*Activate Pull-Up Resistors */ 
	PORTD |= (1<<PD0);  // activating the pull-up resistor for the RX pin (need this set for Serial Comm.... otherwise cannot send rx)
	PORTB |= (1<<PB0); // activate pull-up resistor for pin 0 and pin 1 for buttons (pin 0 and pin 1 will be high) 
	/* Oscillator Calibration */ 
	OSCCAL |= 0xAA; 
}

int main(void)
{
	USARTInit(BAUD_PRESCALE); 
	char data = ' ';
    setup();
	USART_putstring("Hello!\r\n");

	/* Button State */
	uint8_t buttonWasPressed = 0;
	uint8_t pressCount; 
	char pressCountChar[50];
	
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
	
		if(!buttonAvl)
		{
			if (data == keyControl.forward)
			{
				USART_putstring("Moving Forward...\r\n");
				motorControl(data, keyControl);
			}
			else if (data == keyControl.reverse)
			{
				USART_putstring("Moving Reverse...\r\n");
				motorControl(data, keyControl);
			}
			else if (data == keyControl.right)
			{
				USART_putstring("Moving Right...\r\n");
				motorControl(data, keyControl);
			}
			else if (data == keyControl.left)
			{
				USART_putstring("Moving Left...\r\n");
				motorControl(data, keyControl);
			}
			else if (data == keyControl.brake)
			{
				USART_putstring("Stopping...\r\n");
				motorControl(data, keyControl);
			}
			else
			{
				motorControl(data, keyControl);
			}
		}
		
		else if (buttonAvl)	
		{
			if(debounce())
			{
				if(buttonWasPressed == 0)
				{
					buttonWasPressed = 1; 
					PORTB = (1<<PB0)|(1<<PB1); // need to keep port 0 high since pulled-up for push button 
					pressCount++; 
					sprintf(pressCountChar, "Button Pressed %d times...\r\n", pressCount);
					USART_putstring(pressCountChar); 
				}
			}
			else
			{
				buttonWasPressed = 0; 
				PORTB &= (1<<PB0); // need to keep port 0 high since pulled-up for push button 
				//USART_putstring("Button Released...\r\n");
			}
		}	
			
	}
}
