/*
 * GccApplication1.c
 *
 * Created: 8/7/2016 5:04:55 PM
 * Author : Faran Malik
 */ 

#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define buttonAvl 0
#define pwm 

#define timeOut 100000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "usart.h"
#include "control.h"
#include "pushButton.h"

void setup(void)
{
	/* DDRx (Data Direction Register) Configures data direction of port pins */ 
	DDRB |= (1<<DDB3)|(1<<DDB1); // 0b00001000 Make pin 3 of Port B an output pin. Pin 0 and pin 1 kept as inputs for buttons.  
	//DDRC |= (1<<DDC0)|(1<<DDC1); // 0b00000011 Make pin 0 and pin 1 of Port C an output pin
	DDRD |= (1<<DDD1)|(1<<DDD7)|(1<<DDD4)|(1<<DDD5); // 0b10000011 Make pin 7 (motor) and pin 1 (TX) of Port D an output pin. Pin0 (RX) is an input pin so leave alone. 
    /*Activate Pull-Up Resistors */ 
	PORTD |= (1<<PD0);  // activating the pull-up resistor for the RX pin (need this set for Serial Comm.... otherwise cannot send rx)
	PORTB |= (1<<PB0); // activate pull-up resistor for pin 0 and pin 1 for buttons (pin 0 and pin 1 will be high) 
	//PORTA |= (1<<PA6)|(1<<PA7); // activate the pull-up resistor for encoders 
	/* Oscillator Calibration */ 
	OSCCAL |= 0xAA; 
}

void FourChannelPWM(void)
{
	TCCR0 |= (1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS00); //|(1<<COM00)
	TCCR1A |= (1<<WGM10)|(1<<COM1A1)|(1<<COM1B1); //|(1<<WGM12)|(1<<WGM13)|(1<<WGM11)| //using mode 5 Fast PWM 8-Bit TOP 0x00FF this is for OCR1A
	TCCR1B |= (1<<WGM12)|(1<<CS10); // using mode 5 This is for OCR1B I think NOT WORKING
	TCCR2 |=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS20); // left backwards OC2
}


void timerInit()
{
	// set up timer with no prescaling
	TCCR1B |= (1<<CS10); // no prescalar 
	// initialize counter
	TCNT1 = 0; 
}



int main(void)
{
	USARTInit(BAUD_PRESCALE); 

    setup();
	#ifdef pwm
		FourChannelPWM();
	#endif
	timerInit();
	USART_putstring("Hello!\r\n");	
	/* Button State */
	uint8_t buttonWasPressed = 0;
	uint8_t pressCount; 
	char pressCountChar[50];
	char counterChar[10];
	char accelVal[200];
	uint16_t counter = 0; 
	char encoderOne[8];
	char data; //' '; uncomment when using pushbutton 
	
	
	// Define Keys for Control 
	keyCtrl_T keyControl; 
	keyControl.forward = 'w';
	keyControl.reverse = 's'; 
	keyControl.left = 'a';
	keyControl.right = 'd';
	keyControl.brake = 'x';
	
/* Loop Infinitely */ 
    while (1) 
    {
		// Below Testing Timers
	
		if (TCNT1 > 10000)  // 15873
		{
			TCNT1 = 0; 
			counter++;
			if(counter > 1587){ //1000
				counter = 0; 
				PORTB |= (1<<PB0)|(1<<PB1); // need to keep port 0 high since pulled-up for push button
			}
			// PORTB &= ~((1<<PB0)|(1<<PB1)); trying to turn off led not sure if this works. Need to test.
		}
	 // just commented
		//PORTC = 0b00000100; // "break point" 
		//_delay_ms(1000)
		
		if(!buttonAvl)
		{
			//data = USARTReadChar();  // old implementation. Also, does not go to next line unless input take in 
			data = USARTReadCharWithTimeout(timeOut);
			sprintf(accelVal, "Acceleration: OCR0 %d | OCR1 %d | OCR2 %d | OCR1B %d | PINA6 %x | PINA7 %x\r\n", OCR0, OCR1A, OCR2, OCR1B, (PINA && 0b01000000), (PINA && 0x0b10000000));
			USART_putstring(accelVal);
			
			#ifdef pwm
			motorControlPWM(data, keyControl);
			#endif
			
			#ifndef pwm
			motorControl(data, keyControl);
			#endif
		}
			
		else if (buttonAvl)	
		{
			if(debounce())
			{
				if(buttonWasPressed == 0)
				{
					buttonWasPressed = 1; 
					PORTB = (1<<PB0)|(1<<PB1); // need to keep port 0 high since pulled-up for push button 
					pressCount = eeprom_read_byte((uint8_t*)46); // read from memory
					pressCount++; // increment button count
					eeprom_write_byte((uint8_t*)46, pressCount); // write to memory
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
