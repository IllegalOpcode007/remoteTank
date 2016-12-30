/*
 * GccApplication1.c
 *
 * Created: 8/7/2016 5:04:55 PM
 * Author : Faran Malik
 */ 

//#ifndef F_CPU
//#define F_CPU 1000000UL // 1 MHz clock speed
//#endif
#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define kAccelStep 24
#define left l 
#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"


void setup_ports(void)
{
    // USARTWriteChar("Setting Up Ports...\n");
	//DDRx (Data Direction Register) Configures data direction of port pins.  
	DDRB = 0b00001000; // Make pin 3 of Port B an output pin 
	DDRC = 0b00000011; // Make pin 0 and pin 1 of Port C an output pin
	DDRD = 0b10000011; // Make pin 7 (motor), pin 1 (TX) of Port D, pin0 (RX) an output pin
	
	////DDRB = 0xFF;
	//DDRC = 0xFF;
	//DDRD = 0x00; // just added 
//PORTD = 0b00000001;

}


void InitPWM()
{
   /*
   TCCR0 - Timer Counter Control Register (TIMER0)
   -----------------------------------------------
   BITS DESCRIPTION
   

   NO:   NAME   DESCRIPTION
   --------------------------
   BIT 7 : FOC0   Force Output Compare [Not used in this example]
   BIT 6 : WGM00  Wave form generartion mode [SET to 1]
   BIT 5 : COM01  Compare Output Mode        [SET to 1]
   BIT 4 : COM00  Compare Output Mode        [SET to 0]

   BIT 3 : WGM01  Wave form generation mode [SET to 1]
   BIT 2 : CS02   Clock Select               [SET to 0]
   BIT 1 : CS01   Clock Select               [SET to 0]
   BIT 0 : CS00   Clock Select               [SET to 1]

   The above settings are for
   --------------------------

   Timer Clock = CPU Clock (No Prescaling)
   Mode        = Fast PWM
   PWM Output  = Non Inverted
   */

/***** Below needed for acceleration. if uncommented then acceleration will work but will not be able to control both motors *******/ 
	//Left
    //TCCR0 |=(1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS00); //|(1<<COM00)
	//DDRB |= (1<<PB3); // set specific pin to output
	
	//Right 
	//TCCR2 |=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS20); //|(1<<COM00)
	//DDRD |= (1<<PD7); // set specific pin to output
	
	//Right
	//TCCR1A |=(1<<WGM00)|(1<<WGM01)|(1<<COM1A1)|(1<<CS00); //|(1<<COM00) (don't know what I was doing here)
	
}

void accelerate_step(char data) 
{
	uint8_t x = OCR0;
	uint8_t y = OCR2; 
	if (data == 'l') // Go Forward 
	{ 
		y = 0; 
		OCR2 = 0;  
		if(x < (255 - kAccelStep))
			OCR0 = x + kAccelStep;
		else
			OCR0 = 255;
	}
	else if (data == 'o') //Break 
	{
		if(x >= kAccelStep)
		{
			OCR0 = x - kAccelStep;
			
		}
		else if (y >= kAccelStep)
		{
			OCR2 = y - kAccelStep;
		}
		else
		{
			OCR0 = 0;
			OCR2 = 0;
		}
	}
	else if (data == 'p') // Go Back 
	{
		x = 0;
		OCR0 = 0; 
		if(y < (255 - kAccelStep))
		{
			OCR2 = y + kAccelStep;
		}
		else
			OCR2 = 255;
	}
}

void deccelerate_step()
{
	uint8_t x = OCR0;
	if(x >= kAccelStep)
		OCR0 = x - kAccelStep;
	else
		OCR0 = 0;
}


int main(void)
{
	OSCCAL = 0xAA; // Oscillator Calibration
	PORTD |= (1 << PIND0);  // activating the pull-up resistor for the RX pin (need this for sure otherwise no Serial communciation possible.... cannot send rx)
	char data;
	USARTInit(BAUD_PRESCALE); 
	InitPWM(); 
    setup_ports();
    /* Replace with your application code */
    while (1) 
    {
		//PORTC = 0b00000100;
		
		data = USARTReadChar(); // does not go to next line unless input taken in

		if (data == '1')
		{
			USART_putstring("Turning on LED... ");
			USARTWriteChar(data);
			USART_putstring("\n");
			PORTA = 0xFF;
			PORTC = 0b00000001; 
			//_delay_ms(1000);	
		}
		else if (data == '0')
		{
			USART_putstring("Turning off LED... ");
			USARTWriteChar(data);
			USART_putstring("\n");
			PORTA= 0x00;
			PORTC = 0b00000010; 
			//_delay_ms(1000);
		}
		else if (data == 'w')
		{
			USARTWriteChar(data);
			PORTC = 0b00000001; // right forward
			PORTD = 0b00000011; // left backward clear 
			PORTB = 0b00001000; // left forward 
			
			
		}
		else if (data == 's')
		{
			USARTWriteChar(data);
			PORTC = 0b00000110; // right backwards 
			PORTB = 0b00000000; // left forward clear 
			PORTD = 0b10000011; // left backwards
		}
		else if (data == 'd')
		{
			USARTWriteChar(data);
			PORTB = 0b00001000; // left forward 
			PORTC = 0b00000000;
			PORTD = 0b00000011;
		}
		else if (data == 'a')
		{
			USARTWriteChar(data);
			PORTC = 0b00000001; // right forward
			PORTB = 0b00000000;
            PORTD = 0b00000011;
			
		}
		else if (data == 'x')
		{
			USARTWriteChar(data);
			PORTC = 0b00000000;
			PORTB = 0b00000000;
			PORTD = 0b00000011;
		}
		else if (data == 'l')
		{
			USARTWriteChar(data);
			//accelerate_step();
			accelerate_step(data);
		}
		else if (data == 'o')
		{
			USARTWriteChar(data);
			//deccelerate_step();
			accelerate_step(data);
		}
		else if (data == 'p')
		{
			USARTWriteChar(data);
			//deccelerate_step();
			accelerate_step(data);
		}
		
		else if (data == '8') // left forward
		{
			USARTWriteChar(data);
			OCR0 = 255; 
			_delay_ms(1000);
			OCR0 = 0; 
			
		}
		
		else if( data == '9') // left reverse 
		{
			USARTWriteChar(data);
			OCR2 = 255;
			_delay_ms(1000);
			OCR2 = 0;
			
			
		}
		}

		
}




/* Simple LED On/Off 
	else if (data == 'l')
	{
		USARTWriteChar(data);
		while(OCR0<255)
		{
			OCR0+=1; %Changing Duty Cycle 
			_delay_ms(10);
		}
	}
	else if (data == 'o')
	{
		USARTWriteChar(data);
		while(OCR0>0){
			OCR0-=1;
			_delay_ms(10);
		}
	}
*/ 

/*
OCR0 = 0;   // 0% duty cycle: always GND.
OCR0 = 64;  // 25% duty cycle
OCR0 = 128; // 50% duty cycle
OCR0 = 172; // 67% duty cycle
OCR0 = 255; // 100% duty cycle; always VCC.
*/ 