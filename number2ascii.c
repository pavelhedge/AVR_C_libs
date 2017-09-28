/*
 * Переводит число в ASCII кодировку (например для отправки по UART)
 *
 * Чтоб 
 *
 */
 
void hex2ascii(unsigned char number){
	char[MAX + 2] result;
	result[0] = '0';
	result[1] = 'x';
	for(unsigned char i = 2; i < MAX + 2; i++){
		char ci = (unsigned char)(number >> 4 * (MAX - i)) & 0x0F;
		result[i] = ci < 10 ? ci + '0' : ci + 'A';
	}
	return result;
}

void dec2ascii(unsigned char number){
	char[MAX] result;
	for (unsigned char i = 0; i < MAX; i++){
		result[i] = (number/pow(10, MAX - i - 1))%10 + '0';
	}
	return result;
}