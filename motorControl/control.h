/*
 * control.h
 *
 * Created: 12/30/2016 5:17:31 PM
 *  Author: faran
 */ 

#define leftForw (1 << PB3) // OCO 3IN;
#define rightForw (1 << PD5) // 
#define leftRev (1 << PD7) // OC2 4IN
#define rightRev (1 << PD4)
#define clcMsk 0x00

#define rx (1 << PD0)
#define tx (1 << PD1)

uint8_t kAccelStep = 20; 
uint8_t kDeccelStep = 60; // old value 20
uint8_t kCoast = 10;
#define pwmThreshold 60

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
		PORTD = (clcMsk|tx|rx|rightForw); // left backward clear right forward
	}
	else if(data == keyBoard.reverse)
	{
		// Toggle State of Output Pin
		PORTB = clcMsk; // left forward clear
		PORTD = (clcMsk|leftRev|tx|rx|rightRev); // left backwards right rev
	}
	else if(data == keyBoard.left)
	{
		// Toggle State of Output Pin
		PORTB = clcMsk; // left forward clear
		PORTD = (clcMsk|tx|rx|rightForw); // right forward
		
	}
	else if(data == keyBoard.right)
	{
		// Toggle State of Output Pin
		PORTB = (clcMsk|leftForw); // left forward
		PORTD = (clcMsk|tx|rx);
	}
	else if(data == keyBoard.brake)
	{
		// Toggle State of Output Pin
		PORTB = clcMsk;
		PORTD = (clcMsk|rx|tx);
	}
	else
	{
		PORTB = clcMsk;
		PORTD = (clcMsk|rx|tx);
	}
}
void motorControlPWM(char data, keyCtrl_T keyBoard)
{
	if (data == keyBoard.forward)
	{
		if((OCR2 > kDeccelStep) && (OCR1B > kDeccelStep)) // if motor still in reverse, bring to a complete stop
		{
			OCR0 = 0;
			OCR1A = 0;
			OCR2 -= kDeccelStep;
			OCR1B -= kDeccelStep;
		}
		else 
		{
			OCR2 = 0;
			OCR1B = 0;
				if((OCR0 < (255 - kAccelStep)) && (OCR1A < (255 - kAccelStep)))
				{
					if((OCR0 < pwmThreshold) && (OCR1A < pwmThreshold))
					{
						OCR0 = pwmThreshold;
						OCR1A = pwmThreshold;
					}
					OCR0 += kAccelStep;
					OCR1A += kAccelStep;

				}
				else
				{
					OCR0 = 255; // full speed
					OCR1A = 255;
				}
		}
	
	}
	else if(data == keyBoard.reverse) // Left and Right Motor Reverse
	{
		if((OCR0 > kDeccelStep) && (OCR1A > kDeccelStep)) // if motor still in forward, bring to a complete stop
		{
			OCR2 = 0;
			OCR1B = 0;
			OCR0 -= kDeccelStep;
			OCR1A -= kDeccelStep;
		}
		else
		{
			OCR0 = 0;
			OCR1A = 0;
			if((OCR2 < (255 - kAccelStep)) && (OCR1B < (255 - kAccelStep)))
			{
				if((OCR2 < pwmThreshold) && (OCR1B < pwmThreshold))
				{
					OCR2 = pwmThreshold;
					OCR1B = pwmThreshold;
				}
				OCR2 += kAccelStep;
				OCR1B += kAccelStep;
			}
			else
			{
				OCR2 = 255; // full speed
				OCR1B = 255;
			}
		}
	}
	else if(data == keyBoard.left)
	{		
		if((OCR2 > kDeccelStep) && (OCR1B > kDeccelStep)) // if motor still in reverse, bring to a complete stop
		{
			OCR0 = 0;
			OCR1A = 0;
			OCR2 -= kDeccelStep;
			OCR1B -= kDeccelStep;
		}
		else
		{
			OCR2 = 0;
			OCR1B = 0;
			if(OCR0 < (255 - kAccelStep))
			{
				if(OCR0 < pwmThreshold)
				{
					OCR0 = pwmThreshold;
				}
				OCR0 += kAccelStep;
				if (OCR1A > kDeccelStep)
				{
					OCR1A -= kDeccelStep;
				}
			}
			else
			{
				OCR0 = 255; // full speed
				if (OCR1A > kDeccelStep)
				{
					OCR1A -= kDeccelStep;
				}
				else
				{
					OCR1A = 0;
				}
				
			}
		}
	}
	else if(data == keyBoard.right)
	{
		if((OCR2 > kDeccelStep) && (OCR1B > kDeccelStep)) // if motor still in reverse, bring to a complete stop
		{
			OCR0 = 0;
			OCR1A = 0;
			OCR2 -= kDeccelStep;
			OCR1B -= kDeccelStep;
		}
		else
		{
			OCR2 = 0;
			OCR1B = 0;
			if(OCR1A < (255 - kAccelStep))
			{
				if(OCR1A < pwmThreshold)
				{
					OCR1A = pwmThreshold;
				}
				OCR1A += kAccelStep;
				if (OCR0 > kDeccelStep)
				{
					OCR0 -= kDeccelStep;
				}
			}
			else
			{
				OCR1A = 255; // full speed
				if (OCR0 > kDeccelStep)
				{
					OCR0 -= kDeccelStep;
				}
				else
				{
					OCR0 = 0;
				}
				
			}
		}	
	}
	else // else if (data == 0).... sending null character if no activity for certain time
	{
			if((OCR0 > kAccelStep) && (OCR1A > kAccelStep))
			{
				OCR0 -= kCoast;
				OCR1A -= kCoast;
			}
			else if((OCR1B > kAccelStep) && (OCR2 > kAccelStep))
			{
				OCR2 -= kCoast;
				OCR1B -= kCoast;
			}
			else if (OCR0 > kAccelStep)
			{
				OCR0 -= kCoast;
			}
			else if (OCR1A > kAccelStep)
			{
				OCR1A -= kCoast;

			}
			else if (OCR1B > kAccelStep)
			{
				OCR1B -= kCoast;
			}
			else if (OCR2 > kAccelStep)
			{
				OCR2 -= kCoast;
			}
			else
			{
				OCR0 = 0; // Brake
				OCR1A = 0;
				OCR2 = 0;
				OCR1B = 0;
			}
	}
}
	


			/*
			char accelVal[6];
			
			sprintf(accelVal, "Acceleration: %d %d     Encoder One: %x\r\n", OCR0, OCR1A, PINA & 0x40);
USART_putstring(accelVal);*/ 