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



#define uart_rcv_full 0
#define uart_rcv_empty 1
#define uart_send_full 2

#define UART_BUFF_RCV_SIZE 		20
#define UART_BUFF_SEND_SIZE 	20
#define UART_SEND_FREEPLACE (uart_send_out - uart_send_in) >= 0? (uart_send_out - uart_send_in) : (uart_send_out - uart_send_in + UART_BUFF_SEND_SIZE)

/* 

*/
#define baudrate 9600
#define ubrr (unsigned int)(F_CPU / ((baudrate * 16.0) - 1))



unsigned char uart_rcv_buff[UART_BUFF_RCV_SIZE];
unsigned char uart_send_buff[UART_BUFF_SEND_SIZE];

unsigned char *uart_rcv_in = &uart_rcv_buff[0];
unsigned char *uart_rcv_out = &uart_rcv_buff[0];
unsigned char *uart_send_in = &uart_send_buff[0];
unsigned char *uart_send_out = &uart_send_buff[0];
unsigned char uart_flag;


void uart_init(void){
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter */
	UCSR0B = 1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0 | 0 << TXCIE0 | 0 << UDRIE0;
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = 1 << USBS0 | 3 << UCSZ00;	uart_flag = 0;}


char uart_send_char(char byte){
	if (check_bit(uart_flag, uart_send_full)) return FALSE;
	*uart_send_in++ = byte;
	if (uart_send_in == &uart_send_buff[UART_BUFF_SEND_SIZE]){
		uart_send_in = &uart_send_buff[0];
	}
	if(uart_send_in == uart_send_out){
		set_bit(uart_flag, uart_send_full);
	}
	set_bit(UCSR0B, UDRIE0);
	return TRUE;
}


char uart_send_word(int word){
	if (check_bit(uart_flag, uart_send_full) || UART_SEND_FREEPLACE >= sizeof(int)){
		*uart_send_in++ = (char)word>>8;
		if (uart_send_in == &uart_send_buff[UART_BUFF_SEND_SIZE])	uart_send_in = &uart_send_buff[0];
		
		*uart_send_in++ = (char)word;
		if (uart_send_in == &uart_send_buff[UART_BUFF_SEND_SIZE])	uart_send_in = &uart_send_buff[0];
		
		if (uart_send_in == uart_send_out) set_bit(uart_flag, uart_send_full);
		set_bit(UCSR0B, UDRIE0);
		return TRUE;
	}else return FALSE;
}


/*	Для нуль-терминированных строк length = sizeof(string) - 1 */
char uart_send_charr(const char *string, unsigned char length){
	// Если буфер полон или просто меньше размера строки или свободное пространство буфера меньше строки - выход
	if(length > UART_BUFF_SEND_SIZE || check_bit(uart_flag, uart_send_full) || UART_SEND_FREEPLACE >= length)	return FALSE;
	
	// Если строка влезает
	for (unsigned char i = 0; i < length; i++){
		*uart_send_in++ = *string++;
		if (uart_send_in >= &uart_send_buff[UART_BUFF_SEND_SIZE])	uart_send_in = &uart_send_buff[0];
		if(uart_send_in == uart_send_out)	set_bit(uart_flag, uart_send_full);
	}
	set_bit(UCSR0B, UDRIE0);
	return TRUE;
}

char uart_send_pstring(const char *string, unsigned char length){
	// Если буфер полон или просто меньше размера строки или свободное пространство буфера меньше строки - выход
	if(length > UART_BUFF_SEND_SIZE || check_bit(uart_flag, uart_send_full) || UART_SEND_FREEPLACE >= length)	return FALSE;

	// Если строка влезает
	for (unsigned char i = 0; i < length - 1; i++){
		*uart_send_in++ = pgm_read_byte(string++);
		if (uart_send_in >= &uart_send_buff[UART_BUFF_SEND_SIZE])	uart_send_in = &uart_send_buff[0];
		if(uart_send_in == uart_send_out)	set_bit(uart_flag, uart_send_full);
	}
	set_bit(UCSR0B, UDRIE0);
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
	UDR0 = *uart_send_out++;
	clear_bit(uart_flag, uart_send_full);
	if (uart_send_out >= &uart_send_buff[UART_BUFF_SEND_SIZE]){
		uart_send_out = &uart_send_buff[0];			//достиг конца - на начало
	} 
	if (uart_send_in == uart_send_out){
		clear_bit(UCSR0B, UDRIE0);		 			// остановить передачу - отключить прерывание
	}
}


ISR(USART_RX_vect){
	*uart_rcv_in = UDR0;
	clear_bit(uart_flag, uart_rcv_empty);
	if (uart_rcv_in >= &uart_rcv_buff[UART_BUFF_RCV_SIZE]){
		uart_rcv_in = &uart_rcv_buff[0];
	}
	if (uart_rcv_out == uart_rcv_in){
		set_bit(uart_flag, uart_rcv_full);
	}
}