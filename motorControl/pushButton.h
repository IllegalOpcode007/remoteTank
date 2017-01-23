/*
 * pushButton.c
 *
 * Created: 1/7/2017 6:14:03 PM
 *  Author: faran
 */ 

#define btnOne PB0 // may need to change port

#define DEBOUNCE_TIME 1000 // microseconds
uint8_t debounce(void)
{
	if(!(PINB & (1<<btnOne)))
	{
		_delay_us(DEBOUNCE_TIME);
		if(!(PINB & (1<<btnOne))) // button still pressed? 
		{
			return(1);
		}
	}
	return(0);
}