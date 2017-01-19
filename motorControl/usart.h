
#include "avr/interrupt.h"

void USARTInit()
{
   //Set Baud rate
   UBRRL = BAUD_PRESCALE;
   UBRRH = (BAUD_PRESCALE>>8);
   //UCSRB |= (1 << RXCIE); // enable the USART Receive Complete interrupt
   //sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
   
   /*Set Frame Format
   >> Asynchronous mode
   >> No Parity
   >> 1 StopBit
   >> char size 8
   */

/*
#if USE_2X
UCSRA |= _BV(U2X);
#else
UCSRA &= ~(_BV(U2X));
#endif
UCSRC= _BV(UCSZ1) | _BV(UCSZ0); // 8-bit data
*/ 

	UCSRC=(1<<URSEL)|(3<<UCSZ0); // original 

	//Enable The receiver and transmitter
	UCSRB=(1<<RXEN)|(1<<TXEN);
}


char USARTReadChar() 
{
	/* 
	The Receive Complete (RXC) Flag indicates if there are unread data present in the receive buffer. This
	flag is one when unread data exist in the receive buffer, and zero when the receive buffer is empty (i.e.,
	does not contain any unread data).
	*/ 
	while(!(UCSRA & (1<<RXC))) 
	{
		//Do nothing
	}
	return UDR;
}

void USARTWriteChar(char data)
{
	/*
	The UDRE Flag indicates if the transmit buffer (UDR) is ready to receive new data. If UDRE is one, the
	buffer is empty, and therefore ready to be written. The UDRE Flag can generate a Data Register Empty
	interrupt (see description of the UDRIE bit).
	UDRE is set after a reset to indicate that the Transmitter is ready.
	*/ 
	while(!(UCSRA & (1<<UDRE)))
	{
		//Do nothing
	}
	UDR = data;
}

void USART_putstring(char* StringPtr){
	
	while(*StringPtr != 0x00)
	{
		USARTWriteChar(*StringPtr);
		StringPtr++;
	}
}


uint8_t USARTReadCharWithTimeout(uint32_t timeout) {
	{
		uint8_t ret;
		while((!(UCSRA & (1<<RXC)) && (--timeout)))
		{
			// Do nothing.
		}
		if (timeout == 0)
		{
			ret = 0; // Timeout. No command received. Return null character.
		} 
		else {
			ret = UDR; // Command received.
		}
		//timeout = 10; 
		return ret;
	}
}


/*
 Like original program, will echo characters received via the USART. 
 However, because our program is now interrupt driven, we can add in code 
 into the main loop which will be executed when data is not received.
*/
/*
ISR(USART_RXC_vect)
{
	char ReceivedByte;
	ReceivedByte = UDR; // Fetch the received byte value into the variable "ByteReceived"
	UDR = ReceivedByte; // Echo back the received byte back to the computer
}
*/

/******************************About Interrupts*********************************************
Interrupts allow for infrequent "background" tasks to be executed when they occur, without 
posing a run-time penalty of having to poll the hardware until the even occurs. This frees 
up our main loop to take care of the critical code, with the interrupt code pausing the main 
code to execute when the event of interest ocurs.
Interrupts should be made to be as short as possible in execution time. This is because while 
one ISR is executing, others are blocked and thus if another ISR condition occurs while one ISR 
is executing, that ISR event will be missed or delayed.
Because of this, communication ISRs are generally made short by receiving in characters via an 
ISR, and placing them into a buffer which the main code may read at its leisure. This ensures 
that received data will not be missed, while giving the main program time to complete what it is 
currently doing before having to process the input. Similarly, long transmissions may be made by 
placing the data to be sent into a buffer, and have an interrupt send the data as the hardware is 
ready while the main program performs other tasks.
*/
