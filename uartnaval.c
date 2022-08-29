/* navalk
 * uartnaval.c
 *  Author: Navalkishor Kumawat
 *  Created: 16/06/2015 12:28 PM
 */
#include <avr/io.h>
#include "uartnaval.h"


void uart_init(int n)
{	
	UCSRB|=(1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	UCSRC|=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
	//UBRRL=0x47;//11.0592MHz for 9600
	//UBRRL=0x67;//16MHz
	UBRRL=(F_CPU/(16.0*n))-1;
	
}


unsigned char uart_rx()
{
	
	while(!(UCSRA & (1<<RXC)));
	return UDR;

}

void uart_tx(unsigned char ch)
{
	while(!(UCSRA & (1<<UDRE)));
	UDR=ch;
}


/*ISR(USART_RXC_vect)
{	
	c=uart_rx();
	//if(c==0x0d)
	
	okr[i]=c;
	i++;
	if(i==100)
	{	
		i=0;
		
	}
	
}*/
void uart_txstr1(unsigned char str[],int len)
{
    int a=0;

   // while(str[a]!='\0')
   for(a=0;a<len;a++)
	uart_tx(str[a]);
	if(a==len)
	{
		a=0;
		
	}
}
void uart_txstr(unsigned char str[])
{
    int a=0;

    while(str[a]!='\0'){
 //  for(a=0;a<strlen(str);a++)
	uart_tx(str[a]);
	if(a==strlen(str))
	{
		a=0;
		
	} a++; }
}
