#define F_CPU 16000000UL // CPU clock speed settings
// Since the frequency is 16MHz, it is not necessary to set in AVR
 
#include <avr/io.h> // AVR I/O Header Declaration
#include <avr/interrupt.h> // Interrupt header declaration
#include <util/delay.h> // delay function header declaration
 

enum { msec, sec, min };
 
unsigned char FND_DATA[] = { 0, 0, 0, 0, 0 }; // using in mode_2 to save the data
unsigned char FND_DOWN[] = { 0, 0, 0, 0, 0 }; // using in mode_3 to save the data
unsigned char FND_BCD_TBL[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 }; //Table contained Lower BCD code
unsigned char FND_BCD_TBL_H[] = { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90 }; //Table contained Higher BCD code
 
unsigned char LAP_TIME[4][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }; //using in mode_1 to save the Lap time
unsigned char LAP_INDEX = 0;
unsigned char STOP_FLAG = 0; // to indicate the state of stop watch
unsigned char STOP_FLAG2 = 0; // to indicate the state of stop watch
 
unsigned char time_ms = 0, time_s = 0, time_m = 0; //using in timer 3 to calculate time
unsigned char stop_ms = 0, stop_s = 0, stop_m = 0; //using in timer 0 to calculate time
char down_s = 0, down_m = 0, down_ms = 0; //using in mode_3
 
unsigned char buff = 0; // contain packed BCD code for display binary code using GPIO
 
unsigned char mode_sel = 0; // mode selector
unsigned char mode_state = 0; // to indicate the state of mode
unsigned char al_enable = 0; // when 1, enable Alarm, when 0 disable Alarm
unsigned char sw0 = 0, sw1 = 0, sw2 = 0, sw3 = 0, sw6 = 0; //switch flag
unsigned char FND_SEL_CNT = 0, FND_CNT = 0; //using for index
unsigned char al_min = 0, al_sec_u = 0, al_sec_l = 0, al_msec_u = 0, al_msec_l = 0;
 
unsigned char i = 0, j = 0;
unsigned char light = 0;
unsigned char stop_downcount = 0;
unsigned char downcount_setting = 0;
unsigned short cal[4] = { 0 };
unsigned short cal2[4] = { 0 };
unsigned char end = 0;
unsigned char downcount_al_enable = 0;
void print_FND_2bit(unsigned char time_type, unsigned char upper_data, unsigned char lower_data);
 
void mode_0_Basic_Clock();	// call Basic Clock code when mode0
void mode_1_Stop_Watch();
void mode_2_Alarm();
void mode_3_Countdown();
void mode_4_calculator();
void timer0Countup();
void timer0Countdown();
 
ISR(TIMER0_OVF_vect);
ISR(TIMER2_OVF_vect);
ISR(TIMER3_OVF_vect);
 
ISR(INT0_vect);
ISR(INT1_vect);
ISR(INT2_vect);
ISR(INT3_vect);
ISR(INT6_vect);

// In the main statement, there is only the notification function comparison and mode switching
int main()
{
	DDRA = 0xFF; // use port A as output port (use all 0 ~ 7 bits)
	DDRF = 0xFF; // use port F as output port (use all 0 ~ 7 bits)
	DDRC = 0xFF; // use port C as output port (use all 0 ~ 7 bits)
	DDRB = 0xFF; // use port B as output port (use all 0 ~ 7 bits)
	DDRG = 0xFF; // use port G as output port (use all 0 ~ 7 bits)
	DDRE = 0x00; // use port E as output port (use all 0 ~ 7 bits)
	DDRD = 0x00; // use port D as output port (use all 0 ~ 7 bits)
 
	PORTG = 0x00; // initialize to port G 0x00
	PORTE = 0x00; // initialize to port E 0x00
	PORTB = 0xFF; // initialize to port B 0xFF
 
	EICRA = 0xAA; // Interrupt occurs at falling edge of INT0 ~ 3
	EICRB = 0x20; // Interrupt occurs at falling edge of INT6
	EIMSK = 0x4F; // use INT0,1,2,3,6
	EIFR = 0x00; // initialize with flag register 0x00
 
	TCCR3A = 0x00; // set timer/counter3 to normal mode, separate OCnA / OCnB / OCnC
	TCCR3B = 0x02; // 8 min ratio detected by falling edge
	TCCR3C = 0x00; // set Force Output Compare
	TCNT3 = 0x4E20; // save timer/counter register 3 0100 1110 0010 0000
 
	TCCR1A = 0xC1; // set timer/counter1 to mode 1 1100 0001 Set OCnA on compare match
	TCCR1B = 0x03; // 64 division ratio when detected by falling edge
	TCCR1C = 0x00; // set Force Output Compare
	TCNT1 = 0; // save timer/counter register 1 0000 0000 0000 0000
	TIMSK = 0x00; // do not individually enable interrupts in the timer/counter interrupt mask register
	ETIMSK = 0x04; // enable overflow interrupt of extended timer3 individually
	TIFR = 0x01; // overflow operation interrupt operation (automatically cleared)
	ETIFR = 0x04; // if an overflow occurs, TOV3 becomes 1 and an overflow interrupt occurs
 
	print_FND_2bit(msec, 0, 0); // set msec initial value (00)
	print_FND_2bit(sec, 0, 0); // set sec initial value (00)
	print_FND_2bit(min, 0, 0); // set min initial value (00)
 
	sei(); // allow all interrupts
 
	while (1)
	{
		OCR1A = 255; // set output comparison register 1A to 255
 
		switch (mode_sel) // change mode according to mode_sel value
		{
		case 0:
			mode_0_Basic_Clock(); 
			break;
		case 1:
			mode_1_Stop_Watch(); 
			break;
		case 2:
			mode_2_Alarm(); 
			break;
		case 3:
			mode_3_Countdown(); 
			break;
		case 4:
			mode_4_calculator(); 
			break;
		default:
			mode_0_Basic_Clock();
			break;
		}
 
		if (al_enable == 1 && FND_DATA[0] == time_ms % 10 && FND_DATA[1] == time_ms / 10 && FND_DATA[2] == time_s % 10 && FND_DATA[3] == time_s / 10 && FND_DATA[4] == time_m) //¾Ë¶÷ ±â´ÉÀÌ OnÀÌ°í ÀúÀåµÈ ½Ã°£°ú ÇöÀç ½Ã°£ÀÌ ÀÏÄ¡ÇÒ °æ¿ì
		{
			sw1 = 0; // clear sw1
			while (al_enable == 1) // when alarm is executed
			{
				OCR1A = 100; // set the sound size to 100 every 20ms
				_delay_ms(20);
				OCR1A = 255; // set the sound size to 255 every 20ms
				_delay_ms(20);

				if (sw1 == 1) // when sw1 is pressed
				{
				al_enable = 0; // sound off
				}
 
			}
			sw1 = 0; 
		}
 
 
	}
 
	return 0;
}
 
ISR(TIMER0_OVF_vect) // Overflow 0 function (used in mode 1 and 3)
{
	cli(); // Turn off all interrupts

	// Overflow interrupts every count FF~6C(255-108 = 147)
	// 1 / (16Mhz / 1024prescaler) * 147 = 0.01s
	TCNT0 = 0x6C; 
	down_ms++; // used in mode 3
	stop_ms++; // used in mode 1
	timer0Countup();//!!!!!
 
 
	sei(); // all interrupts on
}
 
ISR(TIMER2_OVF_vect) // Overflow 3 Function
{
}
 
ISR(TIMER3_OVF_vect) // adjust in ms, s, m units
{
	cli(); // Turn off all interrupts
	
    // BOFF~FFFF Count (65535 – 45311 = 20224)
	// 1 / (16Mhz / 1024prescaler) * 20224 = 1.29 Overflow Interrupt per 1.29S
	TCNT3 = 0xB0FF;

	time_ms++; 

	if (time_ms >= 99) // if time_ms is over 99 (at 1 second)
	{
 
		if (time_s >= 59) // if time_s is over 59 (at 1 minute)
		{
		time_m++; 
		time_s = 0; 
		}

		else
		time_s++; 
 
		if (time_m>9) // if time_m exceeds 9 (more than 9 minutes)
		{
		time_m = 0;
		}
 
		time_ms = 0; 
	}
	sei(); 
}
 
 
ISR(INT0_vect) // interrupt0 function (for Button1)
{
	cli(); 
 
	if (++mode_sel>4) 
	mode_sel = 0; 
 
	mode_state = 0; 
	sw2 = 0;
	sw3 = 0;
	sw6 = 0;
 
	sei(); 
}
 
ISR(INT1_vect) // ISR Settings for Button2
{
	cli();
	mode_state = !mode_state; 

	if (mode_sel == 2) // in mode 2
	{
		FND_SEL_CNT = 0;
		FND_CNT = FND_DATA[FND_SEL_CNT];
	}

	else if(mode_sel == 3) // in mode 3
	{
		if (downcount_setting == 0)
		{
			downcount_setting = 1;
			end = 0;
		}
		downcount_al_enable = 1;
		FND_SEL_CNT = 0;
		FND_CNT = down_s%10;
	}
	sw1 = 1;
	sei();
}
 
ISR(INT2_vect) // ISR Settings for Button3
{
	cli();
	sw2 = 1;
	sei();
}
 
ISR(INT3_vect) // ISR Settings for Button4
{
	cli();
	sw3 = 1;
	sei();
}
 
ISR(INT6_vect) // ISR Settings for Button5
{
	cli();
	sw6 = 1;
	sei();
}

// Function to put BCD value in each 7-segment place
void print_FND_2bit(unsigned char time_type, unsigned char upper_data, unsigned char lower_data) 
{
	// combine the high and low received BCD values (see the above variable for setting 0 to 9)
	unsigned char data = FND_BCD_TBL_H[upper_data] | FND_BCD_TBL[lower_data]; 
 
	switch (time_type) // select 7-Segment to output
	{
	case msec:
		PORTC = data; // output data value to PORTC
		break;
	case sec:
		PORTF = data; // output data value to PORTF
		break;
	case min:
		PORTA = data; // output data value to PORTA
		break;
	}
 
}

// Mode0 Function
void mode_0_Basic_Clock() 
{
	// msec contains the quotient of high divided by time_ms by 10
	// Low contains the remainder of time_ms
	print_FND_2bit(msec, time_ms / 10, time_ms % 10); 
	_delay_ms(1);

	print_FND_2bit(sec, time_s / 10, time_s % 10); 
	_delay_ms(1);

	print_FND_2bit(min, time_m / 10, time_m % 10); 
	_delay_ms(1);
}
 
// Mode1 Function
void mode_1_Stop_Watch() 
{
 
while (mode_state == 1) 
{
 	PORTG = 0xFF; // LED On
	_delay_ms(50);

	PORTG = 0x00; // LED Off (flashes every 50ms)
	_delay_ms(50);
 
	if (STOP_FLAG == 0) // reset
	{
		STOP_FLAG2 = 1; 
 
		// clear all variables
		stop_ms = 0; 
		stop_s = 0;
		stop_m = 0; 

		TCCR0 = 0x07; // 1024 dispense selection
		
	    // FF to 6C count (255-108 = 147)
		// overflow interrupt every 1 / (16Mhz / 1024prescaler) * 147 = 0.01s
		TCNT0 = 0x6C;

		LAP_INDEX = 0; //!!!!!
 
		STOP_FLAG = 1; 
	}
 
	TIMSK |= 0x01; // output comparison interrupt enable of timer 0
 
	print_FND_2bit(msec, stop_s / 10, stop_s % 10); 
	_delay_ms(1);

	print_FND_2bit(sec, stop_m / 10, stop_m % 10); 
	_delay_ms(1);

	print_FND_2bit(min, mode_sel, LAP_INDEX);
	_delay_ms(1);
 
	if (sw2 == 1) // run when Button3 is pressed
	{
		//Run if LAP_INDEX is less than 4 (to record up to 3 times)
		if (LAP_INDEX < 4) 
		{
			LAP_TIME[LAP_INDEX][0] = stop_s % 10;
			LAP_TIME[LAP_INDEX][1] = stop_s / 10; 
			LAP_TIME[LAP_INDEX][2] = stop_m % 10; 
			LAP_TIME[LAP_INDEX][3] = stop_m / 10; 
		}
 
	if (LAP_INDEX >= 3) // execute if LAP_INDEX is 3 or more
	{
		EIMSK = 0x4B; // Disable interrupt 2 to prevent further numbers
	}
	
	else
		LAP_INDEX++; 
 
	sw2 = 0; // reset Button3
	}

	if (mode_sel != 1) // escape immediately if you change modes
		break;
 
}

	if (STOP_FLAG2 == 1) 
	{
	EIMSK = 0x4F; // external Interrupt 6,3,2,1,0 enable
  	STOP_FLAG = 0; 
 	LAP_INDEX = 0;
	STOP_FLAG2 = 0; // enter 0 in STOP_FLAG2 (cannot be executed again)
	}

	if (sw3 == 1) // when Button4 is pressed
	{
		LAP_INDEX++; 

		if (LAP_INDEX >= 4) 
		{
			LAP_INDEX = 0;
		}
 
		sw3 = 0;
	}
	
	print_FND_2bit(msec, LAP_TIME[LAP_INDEX][1], LAP_TIME[LAP_INDEX][0]); 
	_delay_ms(1);

	print_FND_2bit(sec, LAP_TIME[LAP_INDEX][3], LAP_TIME[LAP_INDEX][2]);
	_delay_ms(1);

	print_FND_2bit(min, mode_sel, LAP_INDEX); 
 
	if (sw6 == 1) // execute when button 5 is pressed(reset)
	{
		
		for (i = 0; i<4; i++)
		{
			for (j = 0; j<4; j++)
			{
				LAP_TIME[i][j] = 0;
			}
		}

		LAP_INDEX = 0;
		stop_ms = 0; 
		stop_s = 0;
		stop_m = 0;
		STOP_FLAG = 0;
 
		EIMSK = 0x47; // external Interrupt 6, 2, 1, 0 enable
 
		sw6 = 0; // reset Button 5
		sw1 = 0; // reset Button 2
	}
 
}
 
// Mode2 Function
void mode_2_Alarm()
{
 
	buff = FND_BCD_TBL_H[FND_DATA[1]] | FND_BCD_TBL[FND_DATA[0]];
	PORTC = buff; // print buff value to port C

	buff = FND_BCD_TBL_H[FND_DATA[3]] | FND_BCD_TBL[FND_DATA[2]];
	PORTF = buff; // print buff value to port F

	print_FND_2bit(min, mode_sel, FND_DATA[4]);
 
	FND_SEL_CNT = 0;
 
	while (mode_state == 1) 
	{
		PORTG = 0xFF; // LED On
		_delay_ms(50);

		PORTG = 0x00; // LED Off (flashes every 50ms)
		_delay_ms(50);
 
		if (sw2 == 1) // run when Button3 is pressed
		{
			FND_SEL_CNT++; // move to left segment

			if (FND_SEL_CNT > 4) // after the 5th segment
			{
				FND_SEL_CNT = 0; // return to the 1st segment
			}

			sw2 = 0; // reset Button3
			FND_CNT = 0; // select another segment to reset the value to 0
		}
 
		if (sw3 == 1) // run when Button4 is pressed
		{
			FND_CNT++;

			// when the selected segment number exceeds 9
			if (FND_CNT > 9) 
			{
				FND_CNT = 0; 
			}
 
			sw3 = 0; //reset Button4
		}
 
		FND_DATA[FND_SEL_CNT] = FND_CNT; // save changed segment

		buff = FND_BCD_TBL_H[FND_DATA[1]] | FND_BCD_TBL[FND_DATA[0]];
		PORTC = buff; // buff output to PORTC

		buff = FND_BCD_TBL_H[FND_DATA[3]] | FND_BCD_TBL[FND_DATA[2]];
		PORTF = buff; // Buff output to PORTF

		print_FND_2bit(min, mode_sel, FND_DATA[4]); 

		if (mode_sel != 2) 
			break;
	}
 
 
	if (sw6 == 1) // when Button 5 is pressed
	{
		al_enable = 1; // enable alarm
		sw6 = 0;
	}
 
}
 
// Mode3 Function
void mode_3_Countdown() 
{
 
	while (mode_state == 1)
	{
		while (downcount_setting)
		{
			if (sw2 == 1) // run when Button3 is pressed
			{
				FND_SEL_CNT++; // move left segment

				if (FND_SEL_CNT>3) 
				{
					FND_SEL_CNT = 0; 
				}
  
				// save current time
				switch (FND_SEL_CNT)
				{ 
				case 0:
					FND_CNT = down_s % 10; // enter 1st segment
					break;
				
				case 1:
					FND_CNT = down_s / 10; // enter 2nd segment
					break;
				
				case 2:
					FND_CNT = down_m % 10; // enter 3rd segment
					break;
				
				case 3:
					FND_CNT = down_m / 10; // enter 4th segment
					break;
				
				default :
					break;
				}
				sw2 = 0; // reset Button3 
			}
 
			if (sw3 == 1) // run when Button4 is pressed
			{
				if (++FND_CNT > 9) 
				{
					FND_CNT = 0; 
				}
				
				FND_DOWN[FND_SEL_CNT] = FND_CNT;
				down_s = FND_DOWN[1] * 10 + FND_DOWN[0]; // enter seconds
				down_m = FND_DOWN[3] * 10 + FND_DOWN[2]; // enter minutes
 
				sw3 = 0; // reset Button4
			}
			
			// 1st and 2nd segment output
 			print_FND_2bit(msec, down_s / 10, down_s % 10); 
			_delay_ms(1);

			// 3rd and 4th segment output
			print_FND_2bit(sec, down_m / 10, down_m % 10); 
			_delay_ms(1);

			// 5th segment output
			print_FND_2bit(min, mode_sel, 0);
			_delay_ms(1);
 
			if (sw6 == 1) // run when Button5 is pressed
			{
				downcount_setting = 0; // down count setting off
				sw6 = 0; // reset Button5
			}
 
			if (mode_sel != 3) 
				break;
		}
 
		if (down_m == 0 && down_s == 0)
		{
 
			while (downcount_al_enable == 1)
			{
				// set sound size to 100 every 20ms
				OCR1A = 100;
				_delay_ms(20);
				
				// set sound size to 255 every 20ms
				OCR1A = 255;
				_delay_ms(20); 
			
				if (sw3 == 1) // run when Button4 is pressed
				{
					end = 1;
					downcount_al_enable = 0;
					sw3 = 0; 
				}
			}
 
			down_m = 0; 
			down_s = 0; 
 
		}
 
		if (STOP_FLAG == 0) 
		{
			STOP_FLAG2 = 1;
 
			down_ms = 0;
			TCCR0 = 0x07; // 1024prescaler
			TCNT0 = 0x6C; // overflow Interrupt every 0.02s
 
			STOP_FLAG = 1; 
		}
 
		TIMSK |= 0x01; 
 
		print_FND_2bit(msec, down_s / 10, down_s % 10); 
		_delay_ms(1);

		print_FND_2bit(sec, down_m / 10, down_m % 10); 
		_delay_ms(1);

		print_FND_2bit(min, mode_sel, 1); 
		_delay_ms(1);
 
 		if (mode_sel != 3) 
			break;
 
		}
 
		if (STOP_FLAG2 == 1) 
		{
			STOP_FLAG = 0;
			STOP_FLAG2 = 0; 
		} 
 
 		print_FND_2bit(msec, down_s / 10, down_s % 10);
		_delay_ms(1);

		print_FND_2bit(sec, down_m / 10, down_m % 10); 
		_delay_ms(1);

		print_FND_2bit(min, mode_sel, 0); 
		_delay_ms(1);
 
} 

// Mode4 Function
void mode_4_calculator()
{
	while (mode_state == 1) 
	{
		PORTG = 0xFF; // LED On
		_delay_ms(50);

		PORTG = 0x00; // LED Off (flashes every 50mns)
		_delay_ms(50);
 
		if (STOP_FLAG == 0)
		{
			FND_SEL_CNT = 0;
			FND_CNT = cal[0];
			cal2[3] = 0;
			STOP_FLAG = 1;
		}
 
		if (sw2 == 1) 
		{
			FND_SEL_CNT++; 
			
			if (FND_SEL_CNT > 3) 
			{
				FND_SEL_CNT = 0; 
			}
			
			sw2 = 0; 
			FND_CNT = cal[FND_SEL_CNT];
		}
 
		if (sw3 == 1) 
		{
			FND_CNT++;
		
			if (FND_CNT > 9) 
			{
				FND_CNT = 0; 
			}

			cal[FND_SEL_CNT] = FND_CNT;
 
			// store the number cal2[0] in the 1st and 2nd segments
			cal2[0] = cal[1] * 10 + cal[0]; 
			// store the number cal2[1] in the 3nd and 4th segments
			cal2[1] = cal[3] * 10 + cal[2]; 
 
			sw3 = 0; 
		}
 
		if (sw6 == 1) 
		{
			cal2[3]++; 
			
			if (cal2[3] > 3) 
				cal2[3] = 0; 
			
			sw6 = 0; 
		}
 
		print_FND_2bit(msec, cal[1], cal[0]); 
		_delay_ms(1);

		print_FND_2bit(sec, cal[3], cal[2]); 
		_delay_ms(1);

		print_FND_2bit(min, mode_sel, cal2[3]);
		_delay_ms(1);
	}
 
	if (sw6 == 1) 
	{
		switch (cal2[3])
		{
		case 0:
			cal2[2] = cal2[1] + cal2[0]; // addition progress
			break;
 
		case 1:
			cal2[2] = cal2[1] - cal2[0]; // subtraction
			break;
 
		case 2:
			cal2[2] = cal2[1] * cal2[0]; // multiplication
			break;
 
		case 3:
			cal2[2] = cal2[1] / cal2[0]; // division
			break;

		default :
			break;
		}

		STOP_FLAG = 0;
		cal2[3] = 4; // set 5th Segment to 4 at completion

		STOP_FLAG = 0;
		i = cal2[2] % 100;
		j = cal2[2] / 100;
		
		print_FND_2bit(msec, i / 10, i % 10); 
		_delay_ms(1);

		print_FND_2bit(sec, j / 10, j % 10);
		_delay_ms(1);

		print_FND_2bit(min, mode_sel, cal2[3]); 
		_delay_ms(1);
		
		sw6 = 0; // reset Button5
	}
 
 
}

void timer0Countdown()
{

}
 
void timer0Countup() 
{
	if (mode_sel == 1)
	{
		if (stop_ms >= 99)
		{
			if (stop_s >= 59)
			{
				stop_m++;
				stop_s = 0;
			}
			else
				stop_s++;
 
			if (stop_m > 59)
			{
				stop_m = 0;
			}
 
			stop_ms = 0;
		}
	}
	
	else if(mode_sel == 3 && downcount_setting == 0&&end == 0)
	{
		if (down_ms >= 99)
		{
			if (down_s == 0)
			{
				down_m--; 
				down_s = 59;
			}
			else
				down_s--;

			down_ms = 0;
		}
	}
}
