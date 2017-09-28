
/*
 *	smartled.S
 *
 *	Created: 20.09.2017 18:12:52
 *  Author: pavelhedge
 *
 *	Выплевывает содержимое буфера в светодиодную ленту через 
 *	любой пин вывода контроллера
 *
 *	Требует скорости контроллера строго 16 или 8 Мгц.
 */ 
#include <avr/io.h>

#define LED_BUFF_SIZE LED_NUM * 3
#define LED_PORT PORTC
#define LED_PIN PORTC0

#define temp r18
#define cntr_byte r19
#define cntr_bit r20


.global smartled
.extern rgb_array

smartled:
// Ускоренный вариант для 8Mhz
ldi XL, lo8(rgb_array)
	ldi XH, hi8(rgb_array)
	ldi cntr_byte, 180

byte_cycle:
	ld temp, X+						; 2 такта
	ldi cntr_bit, 8					; 1 такт

bit_cycle:
	lsl temp						; 1 такт
	brlo set_one	 	 			; 1 такт или 2 при переходе
	sbi LED_PORT - 0x20, LED_PIN	; 2 такта
	cbi LED_PORT - 0x20, LED_PIN	; 2 такта
	rjmp set_zero 					; 2 такта

set_one:
	sbi LED_PORT - 0x20, LED_PIN	; 2 такта
	nop								; 1 такт
	nop								; 1 такт
	cbi LED_PORT - 0x20, LED_PIN	; 2 такта		
			
set_zero:
	dec cntr_bit					; 1 такт
	brne bit_cycle	 				; 1 такт или 2 при переходе
	dec cntr_byte					; 1 такт
	brne byte_cycle					; 1 такт или 2 при переходе

ret

// Более точный вариант для 16Mhz
/*	ldi XL, lo8(rgb_array)
	ldi XH, hi8(rgb_array)
	ldi cntr_byte, 180

byte_cycle:
	ld temp, X+						; 2 такта
	ldi cntr_bit, 8					; 1 такт

bit_cycle:
	sbi LED_PORT - 0x20, LED_PIN			; 2 такта
	lsl temp						; 1 такт
	nop								; 1 такт
	brlo set_one	 	 			; 1 такт или 2 при переходе
	cbi LED_PORT - 0x20, LED_PIN			; 2 такта
	nop								; 1 такт
	nop								; 1 такт
	nop								; 1 такт
	rjmp set_zero 					; 2 такта

set_one:
	nop								; 1 такт
	nop								; 1 такт
	nop								; 1 такт
	nop								; 1 такт
	cbi LED_PORT - 0x20, LED_PIN			; 2 такта

set_zero:
	nop								; 1 такт
	nop								; 1 такт
	nop								; 1 такт
	dec cntr_bit					; 1 такт
	brne bit_cycle	 				; 1 такт или 2 при переходе
	dec cntr_bit					; 1 такт
	brne byte_cycle					; 1 такт или 2 при переходе

ret */

 
