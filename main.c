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

#define T0_CNT_VAL			24
#define	T1_PRESCALAR		64
#define HZ_SAMPLE_CNT		5.0
#define	BASE				(F_CPU/(T1_PRESCALAR*2))
#define SAMPLE_CNT			100
#define PEAK_VAC			600
#define ADC_PVAC_FSCALE		4.25
#define VAC_MUL_FAC			(PEAK_VAC/ADC_PVAC_FSCALE)
#define ADC_REAL_MUL		(5/1024.0)
//#define MV_PER_AMP		0.2			// for ACS712 5A module
#define MV_PER_AMP			0.1			// for ACS712 20A module
//#define MV_PER_AMP		0.066		// for ACS712 30A module

unsigned int adcVac,adcVacMax,adcCurr,adcCurrMax,i,j;
float RmsVoltage,RmsCurrent,freq;
unsigned char str[50],str1[10];

volatile int flag_10ms=0;
volatile int flag_1000ms=0;
volatile int flag_500ms=0;


float getfreq(void);

void init_t0(int t0_cnt_val)
{
	TCCR0 = 0X05;			//	PRESCALAR AS 1024
	TCNT0 = t0_cnt_val;		
	TIMSK |= (1 << TOIE0);  //	Unmask Timer 0 overflow interrupt.
}

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
	TCCR1B = 0x03;					/* capture on falling edge, prescalar 64 */
}

ISR(TIMER0_OVF_vect)
{	
	adcVacMax = 0;
	adcCurrMax = 0;
	
	for(i=0;i<SAMPLE_CNT;i++)
	{
		adcVac = ReadADC(0);
		
		if(adcVac >= adcVacMax)
				adcVacMax = adcVac;
		
		adcCurr = ReadADC(1);
		
		if((adcCurr >= 512) && (adcCurr >= adcCurrMax))
										adcCurrMax = adcCurr;
	}
	TCNT0 = T0_CNT_VAL;
}

ISR(TIMER2_OVF_vect)
{
	static	int count_1000ms=0;
	static	int count_10ms=0;
	static	int count_500ms=0;
	
	if(count_1000ms == 1000)
	{
		flag_1000ms = 1;
		count_1000ms = 0;
	}
	
	if(count_10ms == 10)
	{
		flag_10ms = 1;
		count_10ms = 0;
	}
	
	if(count_500ms == 500)
	{
		flag_500ms = 1;
		count_500ms = 0;
	}
	
	count_500ms++;
	count_1000ms++;
	count_10ms++;
	TCNT2 = 240;
}

int main(void)
{
    /* Replace with your application code */
	InitADC();
	
	uart_init(9600);
	
	init_t0(T0_CNT_VAL);
	
	init_t2();
	
	init_t1_input_capture();
	
	sei();
	
	_delay_ms(100);
	
    while (1) 
    
		
		if(flag_10ms == 1)
		{
			RmsVoltage = adcVacMax * 0.707 * ADC_REAL_MUL * VAC_MUL_FAC;
			RmsCurrent = (adcCurrMax - 512) * 0.707 * ADC_REAL_MUL / MV_PER_AMP;
			
			flag_10ms = 0;
			
		}
		
		else if(flag_1000ms == 1)
		{
			dtostrf(getfreq(),2,1,str1);
			sprintf(str,"\rFrequency is %s Hz",str1);
			uart_txstr(str);
			uart_tx('\r');
			TIMSK = 0x41;
			flag_1000ms = 0;
		}
		
		else if(flag_500ms == 1)
		{
			dtostrf(RmsVoltage,2,1,str1);
			sprintf(str,"\rRMS Voltage is %s",str1);
			uart_txstr(str);
			
			dtostrf(RmsCurrent,2,1,str1);
			sprintf(str," RMS Current is %s A",str1);
			uart_txstr(str);
			
			uart_tx('\r');
			
			flag_500ms = 0;
		}
    
}

float getfreq(void)
{
	TIMSK = 0x00;
	
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