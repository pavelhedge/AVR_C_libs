#include <math.h>
/*
 * Переводит число в ASCII кодировку (например для отправки по UART)
 *
 * Чтоб 
 *
 */

#define HEX2CH_SIZE sizeof(char)*2
#define WORD2CH_SIZE sizeof(int)*2
#define DEC_SIZE 3
 
char* hex2ascii(unsigned char number){
	static char result[HEX2CH_SIZE + 2];
	result[0] = '0';
	result[1] = 'x';
	for(char i = 0; i < HEX2CH_SIZE; i++){
		char ci = (unsigned char)(number >> 4 * (HEX2CH_SIZE - 1 - i)) & 0x0F;
		result[i + 2] = ci < 10 ? ci + '0' : ci + 'A' - 10;
	}
	return result;
}


char* whex2ascii(unsigned int number){
	static char result[WORD2CH_SIZE + 2];
	result[0] = '0';
	result[1] = 'x';
	for(char i = 0; i < WORD2CH_SIZE; i++){
		char ci = (unsigned char)(number >> 4 * (WORD2CH_SIZE - 1 - i)) & 0x0F;
		result[i + 2] = ci < 10 ? ci + '0' : ci + 'A' - 10;
	}
	return result;
}


char* dec2ascii(unsigned char number){
	static char result[DEC_SIZE];
	for (unsigned char i = 0; i < DEC_SIZE; i++){
		char ci = number/(char)pow(10, DEC_SIZE - i - 1);
		if(ci == 0 && ( i == 0 || (i == 1 && result[0] == ' '))) result[i] = ' ';
		else result[i] = ci%10 + '0';
	}
	return result;
}
