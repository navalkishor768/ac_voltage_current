/*
 * adcatmega16.c
 *
 * Created: 2/25/2012 9:18:03 AM
 *  Author: Avinash Gupta
 *  Rewrite by :Navalkishor Kumawat
 *  Rewrite date: 1/13/2015 3:50 PM
 *  Update Date: 5/31/2022  11:01 PM // added addition for ADCH + ADCL
 */
#include <avr/io.h>
#include<util/delay.h>
#include<compat/deprecated.h>
#include "adcatmega16.h"

//Function To Initialize the ADC Module
void InitADC()
{
   ADMUX=(1<<REFS0);                                  	// For Aref=AVcc;
   ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);    //Rrescalar div factor =128

}

/* 

Function To Read ADC Channel

Argument: Channel Number between 0-7
Return Value : Between 0-1023

*/
unsigned int ReadADC(char m)
{	
	unsigned int adval;
	
   
   unsigned char str[30],st[1];
   ADMUX=0b01100000;// ADLAR=1 i.e. left adjusted result // 	8bit
   ADMUX=ADMUX+m;

   //Start Single conversion
   ADCSRA|=(1<<ADSC)|(1<<ADEN);//|(1<<ADATE);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it

   //Note you may be wondering why we have write one to clear it
   //This is standard way of clearing bits in io as said in datasheets.
   //The code writes '1' but it result in setting bit to '0' !!!

   ADCSRA|=(1<<ADIF);
	/*sprintf(str,"ADCH is  %d ",ADCH);
		uart_txstr(str);
	sprintf(str,"ADCL is  %d ",ADCL);
		uart_txstr(str);
		PORTB=ADCL; */
	st[0]=ADCL;
	st[1]=ADCH;
	
	adval=(st[1]<<2)|(st[0]>>6); // adding ADCH+ADCL for making 10 bit number
	
   return adval;
} 
