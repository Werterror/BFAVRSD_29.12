//**************************************************************
//******** FUNCTIONS FOR SERIAL COMMUNICATION USING UART *******
//**************************************************************
//Controller: ATmega32 (8 Mhz internal)
//Compiler: AVR-GCC
//Author: CC Dharmani, Chennai (India)
// www.dharmanitech.com
//Date: 18 April 2009
//**************************************************************

//**************************************************
// ***** SOURCE FILE : UART_routines.c ******
//**************************************************

#include "UART_routines.h"
#include <avr/io.h>
#include <avr/pgmspace.h>


//**************************************************
//Function to receive a single byte
//*************************************************
unsigned char receiveByte( void )
{
	unsigned char data, status;
	
	while(!(UCSRA & (1<<RXC))); 	// Wait for incomming data
	
	status = UCSRA;
	data = UDR;
	
	return(data);
}

//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte( unsigned char data )
{
	while ( !(UCSRA & (1<<UDRE)) )
		; 			                /* Wait for empty transmit buffer */
	UDR = data; 			        /* Start transmition */
}


//***************************************************
//Function to transmit hex format data
//first argument indicates type: CHAR, INT or LONG
//Second argument is the data to be displayed
//***************************************************
void transmitHex( unsigned char dataType, unsigned long data )
{
unsigned char count, i, temp;
unsigned char dataString[] = "0x        ";

if (dataType == CHAR) count = 2;
if (dataType == INT) count = 4;
if (dataType == LONG) count = 8;

for(i=count; i>0; i--)
{
  temp = data % 16;
  if((temp>=0) && (temp<10)) dataString [i+1] = temp + 0x30;
  else dataString [i+1] = (temp - 10) + 0x41;

  data = data/16;
}

transmitString (dataString);
}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(char* string)
{
  while (pgm_read_byte(&(*string)))
   transmitByte(pgm_read_byte(&(*string++)));
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(unsigned char* string)
{
  while (*string)
   transmitByte(*string++);
}

//************ END ***** www.dharmanitech.com *******
void uart0_init(unsigned int ubrr){
	//Ustawienie BAUD RATE
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	//W��czenie TX i RX
	UCSRB = (1<<RXEN) | (1<<TXEN);
	// Ustawienie ramki, 8 bitow | 1 bit stopu
	UCSRC = (1<<URSEL) | (3<<UCSZ0);
	//Tryb asynchroniczny normalny
	UCSRA &= ~(1<<U2X);
}
void uartdoubletransmit(void){
	UCSRA |= (1<<U2X);
}
