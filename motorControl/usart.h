void USARTInit()
{
   //Set Baud rate
   UBRRL = BAUD_PRESCALE;
   UBRRH = (BAUD_PRESCALE>>8);
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
	while(!(UCSRA & (1<<RXC)))
	{
		//Do nothing
	}
	return UDR;
}

void USARTWriteChar(char data)
{
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