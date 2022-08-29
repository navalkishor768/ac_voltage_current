/*
 * adcatmega16.h
 *
 * Created: 2/25/2012 9:17:12 AM
 *  Author: Avinash Gupta
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>


//Threshold Values For Sensor Triggering
#define RTHRES 195
#define CTHRES 275
#define LTHRES 195

//Function To Initialize the ADC Module
void InitADC();

/* 

Function To Read ADC Channel

Argument: Channel Number between 0-7
Return Value : Between 0-1023

*/
unsigned int ReadADC(char m);



#endif /* ADC_H_ */
