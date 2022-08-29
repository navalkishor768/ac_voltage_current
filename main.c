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

#define T0_CNT_VAL		24
#define SAMPLE_CNT		50
#define PEAK_VAC			600
#define ADC_PVAC_FSCALE		4.25
#define VAC_MUL_FAC			(PEAK_VAC/ADC_PVAC_FSCALE)
#define ADC_REAL_MUL		(5/1024.0)
//#define MV_PER_AMP		0.2			// for ACS712 5A module
#define MV_PER_AMP		0.1			// for ACS712 20A module
//#define MV_PER_AMP			0.066		// for ACS712 30A module

unsigned int adcVac,adcVacMax,adcCurr,adcCurrMax,i;
float RmsVoltage,RmsCurrent;
unsigned char str[50],str1[10];


void init_t0(int t0_cnt_val)
{
	TCCR0 = 0X05;			//	PRESCALAR AS 1024
	TCNT0 = t0_cnt_val;		
	TIMSK |= (1 << TOIE0);  //	Unmask Timer 0 overflow interrupt.
}

ISR(TIMER0_OVF_vect)
{
	adcVacMax = 0;
	adcCurrMax = 0;
	
	for(i=0;i<SAMPLE_CNT;i++)
	{
		adcVac = ReadADC(0);
		adcCurr = ReadADC(1);
		
		if(adcVac > adcVacMax)
				adcVacMax = adcVac;
		
		if(adcCurr > adcCurrMax)
				adcCurrMax = adcCurr;
	}
	
	
	TCNT0 = T0_CNT_VAL;
}


int main(void)
{
    /* Replace with your application code */
	InitADC();
	
	uart_init(9600);
	
	init_t0(T0_CNT_VAL);
	
	sei();
	
	_delay_ms(100);
	
    while (1) 
    {
		RmsVoltage = adcVacMax * 0.707 * ADC_REAL_MUL * VAC_MUL_FAC;
		dtostrf(RmsVoltage,2,1,str1);
		sprintf(str,"RMS Voltage is %s",str1);
		uart_txstr(str);
		
		RmsCurrent = (adcCurrMax - 512) * 0.707 * ADC_REAL_MUL / MV_PER_AMP;
		dtostrf(RmsCurrent,2,1,str1);
		sprintf(str," RMS Current is %s A",str1);
		uart_txstr(str);
		
		
		uart_tx('\r');
		_delay_ms(500);
    }
}

