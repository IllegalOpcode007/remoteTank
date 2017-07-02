/*
 * control.h
 *
 * Created: 12/30/2016 5:17:31 PM
 *  Author: faran
 */ 

#define leftForw (1 << PB3) // OCO 3IN
#define rightForw (1 << PC0) // 
#define leftRev (1 << PD7) // OC2 4IN
#define rightRev (1 << PC1)
#define clcMsk 0x00

#define rx (1 << PD0)
#define tx (1 << PD1)

typedef struct keyCtrl_S
{
	char forward;
	char reverse;
	char left;
	char right;
	char brake;
} keyCtrl_T;

void motorControl(char data, keyCtrl_T keyBoard)
{
	if(data ==	keyBoard.forward)
	{
		// Toggle State of Output Pin
		PORTB = (clcMsk|leftForw); // left forward
		PORTC = (clcMsk|rightForw); // right forward
		PORTD = (clcMsk|tx|rx); // left backward clear
	}
	else if(data == keyBoard.reverse)
	{
		// Toggle State of Output Pin
		PORTB = clcMsk; // left forward clear
		PORTC = (clcMsk|rightRev); // right backwards
		PORTD = (clcMsk|leftRev|tx|rx); // left backwards
	}
	else if(data == keyBoard.left)
	{
		// Toggle State of Output Pin
		PORTC = (clcMsk|rightForw); // right forward
		PORTB = clcMsk; // left forward clear
		PORTD = (clcMsk|tx|rx);
		
	}
	else if(data == keyBoard.right)
	{
		// Toggle State of Output Pin
		PORTB = (clcMsk|leftForw); // left forward
		PORTC = clcMsk;
		PORTD = (clcMsk|tx|rx);
	}
	else if(data == keyBoard.brake)
	{
		// Toggle State of Output Pin
		PORTC = clcMsk;
		PORTB = clcMsk;
		PORTD = (clcMsk|rx|tx);
	}
	else
	{
		PORTC = clcMsk;
		PORTB = clcMsk;
		PORTD = (clcMsk|rx|tx);
	}
}



