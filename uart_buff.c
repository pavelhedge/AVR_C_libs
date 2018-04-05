/*
 * Используется кольцевой буфер.
 *
 * Данные добавляются в буфер и считываются из него по указателю
 * Если указатель доходит до конца буфера - ставится в начало
 * Если указатели совпадают при заполнении, ставится флаг переполнения
 * Если совпадают при выгрузке - флаг о пустом буфере или отключается передача
 *
 * 		in	0	1	2	3	4	5	6	7	8	9
 *					-
 *		out	0	1	2	3	4	5	6	7	8	9
 *							-
 *
 */

#include <avr/pgmspace.h>

#define uart_rcv_full 0
#define uart_rcv_empty 1
#define uart_send_full 2

#define TRUE 	1
#define FALSE	0

#define UART_BUFF_RCV_SIZE 		20
#define UART_BUFF_SEND_SIZE 	40

#define baudrate 9600
#define ubrr (unsigned int)(F_CPU / ((baudrate * 16.0) - 1))



unsigned char uart_rcv_buff[UART_BUFF_RCV_SIZE];
unsigned char uart_send_buff[UART_BUFF_SEND_SIZE];

unsigned char *uart_rcv_in = &uart_rcv_buff[0];
unsigned char *uart_rcv_out = &uart_rcv_buff[0];
unsigned char *uart_send_in = &uart_send_buff[0];
unsigned char *uart_send_out = &uart_send_buff[0];
unsigned char uart_flag;

#if (defined __AVR_ATmega48__)||(defined __AVR_ATmega48A__)||(defined __AVR_ATmega48P__)||(defined __AVR_ATmega48PA__)
	#define UDR UDR0
	#define UBRRH UBRR0H
	#define UBRRL UBRR0L
	#define UCSRA UCSR0A
		#define RXC RXC0
		#define TXC TXC0
		#define UDRE UDRE0
	#define UCSRB UCSR0B
		#define RXEN RXEN0
		#define TXEN TXEN0
		#define RXCIE RXCIE0
		#define TXCIE TXCIE0
		#define UDRIE UDRIE0
	#define UCSRC UCSR0C
		#define USBS USBS0
		#define UCSZ0 UCSZ00
		#define UCSZ1 UCSZ01
		#define UCSZ2 UCSZ02
#endif 


void uart_init(void){
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	//Enable receiver and transmitter */
	UCSRB = 1 << RXEN | 1 << TXEN | 1 << RXCIE | 0 << TXCIE | 0 << UDRIE;
	/* Set frame format: 8data, 2stop bit */
#if (defined __AVR_ATmega48__)||(defined __AVR_ATmega48A__)||(defined __AVR_ATmega48P__)||(defined __AVR_ATmega48PA__)
	UCSRC = 1 << USBS | 3 << UCSZ0;
#else 
	UCSRC = 1 << URSEL | 1 << USBS | 3 << UCSZ0;
#endif	uart_flag = 0;	sei();}


char uart_send_freeplace(void){
	signed char uart_send_free = uart_send_out - uart_send_in;
	return uart_send_free > 0 ? uart_send_free : uart_send_free + UART_BUFF_SEND_SIZE;
}


char uart_send_char(uint8_t byte){
	if (check_bit(uart_flag, uart_send_full)) return FALSE;
	*uart_send_in++ = byte;
	if (uart_send_in == &uart_send_buff[UART_BUFF_SEND_SIZE]){
		uart_send_in = &uart_send_buff[0];
	}
	if(uart_send_in == uart_send_out){
		set_bit(uart_flag, uart_send_full);
	}
	set_bit(UCSRB, UDRIE);
	return TRUE;
}


char uart_send_word(int word){
	if (check_bit(uart_flag, uart_send_full) || uart_send_freeplace() >= sizeof(int)){
		*uart_send_in++ = (char)word>>8;
		if (uart_send_in == &uart_send_buff[UART_BUFF_SEND_SIZE])	uart_send_in = &uart_send_buff[0];
		
		*uart_send_in++ = (char)word;
		if (uart_send_in == &uart_send_buff[UART_BUFF_SEND_SIZE])	uart_send_in = &uart_send_buff[0];
		
		if (uart_send_in == uart_send_out) set_bit(uart_flag, uart_send_full);
		set_bit(UCSRB, UDRIE);
		return TRUE;
	}else return FALSE;
}


/*	Для нуль-терминированных строк length = sizeof(string) - 1 */
char uart_send_string(const char *string, unsigned char length){
	// Если буфер полон или просто меньше размера строки или свободное пространство буфера меньше строки - выход
	if(length > UART_BUFF_SEND_SIZE || check_bit(uart_flag, uart_send_full) || uart_send_freeplace() <= length)	return FALSE;
	
	// Если строка влезает
	for (unsigned char i = 0; i < length; i++){
		*uart_send_in++ = *string++;
		if (uart_send_in >= &uart_send_buff[UART_BUFF_SEND_SIZE])	uart_send_in = &uart_send_buff[0];
		if(uart_send_in == uart_send_out)	set_bit(uart_flag, uart_send_full);
	}
	set_bit(UCSRB, UDRIE);
	return TRUE;
}

char uart_send_pstring(const char *string, unsigned char length){
	// Если буфер полон или просто меньше размера строки или свободное пространство буфера меньше строки - выход
	if(length > UART_BUFF_SEND_SIZE || check_bit(uart_flag, uart_send_full) || uart_send_freeplace() <= length)	return FALSE;

	// Если строка влезает
	for (unsigned char i = 0; i < length - 1; i++){
		*uart_send_in++ = pgm_read_byte(string++);
		if (uart_send_in >= &uart_send_buff[UART_BUFF_SEND_SIZE])	uart_send_in = &uart_send_buff[0];
		if(uart_send_in == uart_send_out)	set_bit(uart_flag, uart_send_full);
	}
	set_bit(UCSRB, UDRIE);
	return TRUE;
}

char uart_recieve_byte(){
	unsigned char byte = *uart_rcv_out++;
	clear_bit(uart_flag, uart_rcv_full);
	if (uart_rcv_out >= &uart_rcv_buff[UART_BUFF_RCV_SIZE]){
		uart_rcv_out = &uart_rcv_buff[0];
	}
	if (uart_rcv_out == uart_rcv_in){
		set_bit(uart_flag, uart_rcv_empty);
	}
	return byte;
}




ISR(USART_UDRE_vect){
	UDR = *uart_send_out++;
	clear_bit(uart_flag, uart_send_full);
	if (uart_send_out >= &uart_send_buff[UART_BUFF_SEND_SIZE]){
		uart_send_out = &uart_send_buff[0];			//достиг конца - на начало
	} 
	if (uart_send_in == uart_send_out){
		clear_bit(UCSRB, UDRIE);		 			// остановить передачу - отключить прерывание
	}
}


ISR(USART_RX_vect){
	*uart_rcv_in = UDR;
	clear_bit(uart_flag, uart_rcv_empty);
	if (uart_rcv_in >= &uart_rcv_buff[UART_BUFF_RCV_SIZE]){
		uart_rcv_in = &uart_rcv_buff[0];
	}
	if (uart_rcv_out == uart_rcv_in){
		set_bit(uart_flag, uart_rcv_full);
	}
}