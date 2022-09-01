/*
 * ADC  Timer Interrupt.c
 *
 * Created: 29-Aug-22 8:30:55 PM
 * Author : admin
 */ 
#undef F_CPU
#define F_CPU			16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "adcatmega16.h"
#include "adcatmega16.C"
#include "uartnaval.h"
#include "uartnaval.c"
#include "lcdgdheerajat8.h"
#include "lcdgdheerajat8.c"

#define T0_CNT_VAL			30
#define	T1_PRESCALAR		1024
#define HZ_SAMPLE_CNT		5
#define	BASE				(F_CPU/(T1_PRESCALAR*2))
#define SAMPLE_CNT			100
#define PEAK_VAC			311
#define ADC_PVAC_FSCALE		2.205
#define SCALE_MUL_FAC		(PEAK_VAC/ADC_PVAC_FSCALE)
#define ADC_REAL_MUL		(5/1024.0)
//#define MV_PER_AMP		0.2			// for ACS712 5A module
#define MV_PER_AMP			0.1			// for ACS712 20A module
//#define MV_PER_AMP		0.066		// for ACS712 30A module
#define error_per			1.1

unsigned int adcVac,adcVacMax,adcCurr,i,j,VacSampleCNT = 0,CurrSampleCNT=0,sampleCNT=0;
unsigned long int adcVacSq=0,adcCurrMax=0;
float RmsVoltage,RmsCurrent,freq;
unsigned char str[50],str1[10];

volatile int flag_1000ms=0;
volatile int flag_500ms=0;


float getfreq(void);

void init_t2()
{
	TCCR2 = 0x07;			//	PRESCALAR AS 1024
	TCNT2 = 240;			//  initialize counter for 1 ms
	TIMSK |= (1 << TOIE2);	//	Unmask Timer 2 overflow interrupt.
}

void init_t1_input_capture(void)	 /* function to initialize Timer1 input capture  */
{
	TCCR1A = 0;
	TIFR = (1<<ICF1);				/* clear input capture flag */
	TCCR1B = 0x05;					/* capture on falling edge, prescalar 1024 */
}

ISR(TIMER2_OVF_vect)
{
	static unsigned int count_1000ms=0;
	static unsigned int count_500ms=0;
	
	static unsigned long int adcVacSqSmpl=0,locSampleCNT=0,adcCurrSmpl=0,locCurrSmplCNT=0;
	adcVac = ReadADC(0);
	
	if(adcVac >= 200)	
	{
		adcVacSqSmpl += adcVac * ADC_REAL_MUL * adcVac * ADC_REAL_MUL;
		locSampleCNT++;
	}
	
	adcCurr = ReadADC(1);
	if(adcCurr >= adcCurrSmpl)
	{
		adcCurrSmpl = adcCurr;
		locCurrSmplCNT++;
	}
	
	if(count_1000ms == 1000)
	{	
		adcCurrMax = adcCurrSmpl;
		adcVacSq = adcVacSqSmpl;
		adcVacSqSmpl = 0;
		adcCurrSmpl = 0;
		sampleCNT = locSampleCNT;
		CurrSampleCNT = locCurrSmplCNT;
		locSampleCNT=0;
		locCurrSmplCNT = 0;
		flag_1000ms = 1;
		count_1000ms = 0;
	}
	
	if(count_500ms == 500)
	{	
		flag_500ms = 1;
		count_500ms = 0;
	}
	
	count_500ms++;
	count_1000ms++;
	TCNT2 = 240;
}

int main(void)
{
    /* Replace with your application code */
	InitADC();
	
	uart_init(9600);
	
	lcd_init(LCD_DISP_ON);
	//_delay_ms(100);
	
	init_t2();
	
	init_t1_input_capture();
	
	sei();
	
	//_delay_ms(100);
	
    while (1) 
    
		
		if(flag_1000ms == 1)
		{	
			RmsVoltage = SCALE_MUL_FAC * sqrt(adcVacSq / sampleCNT) * error_per;
			
			RmsCurrent = (adcCurrMax - 512) * 0.707 * ADC_REAL_MUL / MV_PER_AMP;
			
			/*
			Convert Float Values to string to transmit on UART
			*/
			
			dtostrf(RmsVoltage,2,1,str1);
			sprintf(str,"\rRMS Voltage = %s",str1);
			uart_txstr(str);
			
			dtostrf(RmsCurrent,2,1,str1);
			sprintf(str," RMS Current = %s A",str1);
			uart_txstr(str);
			
			freq = getfreq();
			dtostrf(freq,2,1,str1);
			sprintf(str,"\rFrequency = %s Hz",str1);
			uart_txstr(str);
			uart_tx('\r');
			TIMSK = 0x40;		// enable timer interrupts
			
			flag_1000ms = 0;
		}
		
		else if(flag_500ms == 1)
		{
			/* 
			Display RMS Voltage, Current and Freq on 16x2 LCD
			*/
			lcd_clrscr();
			dtostrf(RmsVoltage,2,1,str1);
			lcd_putsxy(0,0,"VRMS=");
			lcd_putsxy(5,0,str1);
			
			dtostrf(RmsCurrent,2,1,str1);
			lcd_putsxy(5,1,"000");
			lcd_putsxy(0,1,"IRMS=");
			lcd_putsxy(5,1,str1);
			lcd_putsxy(9,1,"A");
			
			dtostrf(freq,2,1,str1);
			lcd_putsxy(11,1," F=");
			lcd_putsxy(14,1,str1);
			
			flag_500ms = 0;
		}
    
}

float getfreq(void)
{
	TIMSK = 0x00;		// disable timer interrupts for input capture reading
	
	unsigned int timePEdge=0, timeEdge1;
	
	for(j=0;j<HZ_SAMPLE_CNT;j++){
		
		/* calculating time period between 2 consecutive falling edge */
		
		while ((TIFR&(1<<ICF1)) == 0);						/* monitor for capture*/
		timeEdge1 = ICR1;
		
		TIFR = (1<<ICF1);									/* clear capture flag */
		
		while ((TIFR&(1<<ICF1)) == 0);						/* monitor for next EDGE edge capture */
		timePEdge += ICR1 - timeEdge1;						/* period= recent capture - previous capture */
		
		TIFR = (1<<ICF1);									/* clear capture flag */
	}
	return (BASE/(timePEdge/HZ_SAMPLE_CNT));
}