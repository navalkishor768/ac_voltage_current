/*
 * uartnaval.h
 *
 * Created: 16/06/2015 12:23 PM
 *  Author: Navalkishor Kumawat
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>


//Function To Initialize the UART Module
void uart_init(int n);

//Function to tx a char
void uart_tx(unsigned char ch);

//Function to tx a string of length len
void uart_txstr1(unsigned char str[],int len);

void uart_txstr(unsigned char str[]);

//Function to receive a char
unsigned char uart_rx();

#endif /* UART_H_ */
