/*------------------------------------------------*/
/* UART functions                                 */
/*------------------------------------------------*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#define	BAUD		115200


typedef struct {
	uint8_t	wi, ri, ct;
	uint8_t buff[64];
} FIFO;
static
volatile FIFO TxFifo, RxFifo;



/* Initialize UART */

void uart_init (unsigned int ubrr)
{
// 	UCSR0B = 0;
// 
// 	PORTE |= _BV(1); DDRE |= _BV(1);	/* Set TXD as output */
// 	DDRE &= ~_BV(0); PORTE &= ~_BV(0); 	/* Set RXD as input */
// 
// 	RxFifo.ct = 0; RxFifo.ri = 0; RxFifo.wi = 0;
// 	TxFifo.ct = 0; TxFifo.ri = 0; TxFifo.wi = 0;
// 
// 	UBRR0L = F_CPU / BAUD / 16 - 1;
// 	UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(TXEN0);
// 	
	
	//Set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	UCSR0C = 0x06; //set asynchronous, no parity, one stop bit, 8 bit transfer.
	
	UCSR0B |= (1 << RXCIE0); //set RX interrupt on
}


/* Get a received character */

uint8_t uart_test (void)
{
	return RxFifo.ct;
}


uint8_t uart_get (void)
{
	uint8_t d, i;


	while (RxFifo.ct == 0) ;
	i = RxFifo.ri;
	d = RxFifo.buff[i];
	cli();
	RxFifo.ct--;
	sei();
	RxFifo.ri = (i + 1) % sizeof RxFifo.buff;

	return d;
}


/* Put a character to transmit */

void uart_put (uint8_t d)
{
	uint8_t i;


	while (TxFifo.ct >= sizeof TxFifo.buff) ;
	i = TxFifo.wi;
	TxFifo.buff[i] = d;
	cli();
	TxFifo.ct++;
	UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(TXEN0) | _BV(UDRIE0);
	sei();
	TxFifo.wi = (i + 1) % sizeof TxFifo.buff;
}


/* UART RXC interrupt */

ISR(USART0_RX_vect)
{
	uint8_t d, n, i;


	d = UDR0;
	n = RxFifo.ct;
	if (n < sizeof RxFifo.buff) {
		RxFifo.ct = ++n;
		i = RxFifo.wi;
		RxFifo.buff[i] = d;
		RxFifo.wi = (i + 1) % sizeof RxFifo.buff;
	}
}


/* UART UDRE interrupt */

ISR(USART0_UDRE_vect)
{
	uint8_t n, i;


	n = TxFifo.ct;
	if (n) {
		TxFifo.ct = --n;
		i = TxFifo.ri;
		UDR0 = TxFifo.buff[i];
		TxFifo.ri = (i + 1) % sizeof TxFifo.buff;
	}
	if (n == 0) UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(TXEN0);
}

