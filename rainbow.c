// Библиотека всяких программок и методов для вывода на светодиодную ленту.

/*
#define in_range(value, lower, upper) value >= upper? lower : value
#define in_range(value, upper)	value >= upper? 0 : value

char in_range(char value, char lower, char upper){
	if(value <= lower) return upper;
	else if(value >= upper) return lower;
	else return value;
}*/

#define FALSE	0
#define TRUE	1

//	Количество светодиодов в циферблате
#define LED_NUM 60

//	Ширина секторов часовой и минутной стрелок
#define HOUR_WIDTH 		2
#define MINUTE_WIDTH 	1
#define BGND			0x80

#include "async_rtc.c"
#include "smartled.h"
#include "hsv2rgb.c"
#include <stdlib.h>

unsigned char rgb_array[LED_NUM][3];

unsigned char hour_pos;				// Позиция часовой стрелки

/*************************************************************
		Часы с радугой 1
*************************************************************/

void rainbow_clock1(void){
/*
 *	Показывает время, закрашивая сектор по часовой стрелке от часовой
 *	до минутной стрелки чем-нибудь типа радуги
 *
 *
 */
	
	signed char sector_lenght;
	unsigned char sector_over; // сектор переходит через 12 часов, границу.
	//unsigned char sector_pos; //не помню, зачем эта переменная
	
	hour_pos = t.hour%12*5 + t.minute / 12;
	sector_lenght = t.minute - hour_pos;
	if(sector_lenght < 0){
		sector_lenght += LED_NUM;
		sector_over = TRUE;
	} else sector_over = FALSE;
	
	for(unsigned char i = 0; i < LED_NUM; i++){
		if(sector_over? (i <= t.minute || i >= hour_pos) : (i <= t.minute && i >= hour_pos)){
			unsigned char hue = (unsigned char)((unsigned short)(HUEMAX * (t.minute - i)/sector_lenght));
			hsv2rgb(hue, SATMAX, VALMAX, rgb_array[i]);
		} else if(i == t.second){
			rgb_array[i][0] = 0;
			rgb_array[i][1] = 0;
		 	rgb_array[i][2] = 0;
		} else {
			rgb_array[i][0] = BGND;
			rgb_array[i][1] = BGND;
		 	rgb_array[i][2] = BGND;
		}
	}
}
/*************************************************************
		Простые часы для проверки
*************************************************************/
void plain_clock(void){
	hour_pos = t.hour%12*5 + t.minute / 12;
	
	for (unsigned char i = 0; i < LED_NUM; i++){
		if (i == t.second) {
			rgb_array[i][0] = 0x00;
			rgb_array[i][1] = 0xFF;
			rgb_array[i][2] = 0x00;
			} else if (abs(i - t.minute) < 2){
			rgb_array[i][0] = 0xFF;
			rgb_array[i][1] = 0x00;
			rgb_array[i][2] = 0x00;
			} else if (abs(i - hour_pos) < 3){
			rgb_array[i][0] = 0x00;
			rgb_array[i][1] = 0x00;
			rgb_array[i][2] = 0xFF;
			} else {
			rgb_array[i][0] = 0x00;
			rgb_array[i][1] = 0x00;
			rgb_array[i][2] = 0x00;
		}
	}
}

/*************************************************************
		Часы с радугой 2
*************************************************************/
	

void rainbow_clock2(void){
/*
 *	Весь циферблат заливается переливом чистого тона, вращающимся
 *	вместе с воображаемой секундной стрелкой.
 *
 *	Час отмечается плавным провалом яркости (черный), минута - плавным
 *  провалом насыщенности (белый).
 *	Изменения яркости и насыщенности читаются в зависимости от 
 *	удаления от стрелки
 */


	unsigned char (*rgb_ptr)[3] = rgb_array + t.second + 1;
	unsigned char i = 0;


	while(rgb_ptr != rgb_array + t.second){
		char val = (abs(t.hour - i) > HOUR_WIDTH)? VALMAX : VALMAX * abs(t.hour - i) / HOUR_WIDTH;
		char sat = (abs(t.minute - i) > MINUTE_WIDTH)? SATMAX : SATMAX * (t.minute - i) / MINUTE_WIDTH;
		hsv2rgb(3*i++, sat, val, rgb_ptr++);
		if(rgb_ptr == &rgb_array + LED_NUM) rgb_ptr = &rgb_array;
	}
}



/*
void rainbow_clock3(){
/ *
 * 	циферблат белый, с провалами рандомной радуги, 5 диодов - час
 *	3 - минута, шириной в 1 диод - секунда. Удобней поставить HUEMAX = 255
 *
 *	черный циферблат с белыми стрелками или наоборот.
 * /
 
	hour_pos = t.hour%12*5 + t.minute / 12;
 	char hour_rand = rand();
 	char minute_rand = rand();
 	for(unsigned char i = 0; i < LED_NUM; i++){
		if(i == t.second) {
			hsv2rgb(rand(), SATMAX, VALMAX);
		}else if(abs(t.minute - i) < MINUTE_WIDTH){
			char hue = minute_rand + (i + MINUTE_WIDTH - t.minute)*MINUTE_WIDTH*10;
			hsv2rgb(minute_rand + (i - t.minute + MINUTE_WIDTH)*MINUTE_WIDTH*10;
		}else if(abs(t.hour - i) < HOUR_WIDTH){
			hsv2rgb();
		}else{
			rgb_array[i][0] = BGND;
			rgb_array[i][1] = BGND;
		 	rgb_array[i][2] = BGND;
		}
	}
 
 
}*/
/*************************************************************
		Просто заливка радугой без часов
*************************************************************/
void rainbow_fill(void){
	for (unsigned char i = 0; i < LED_NUM; i++){
		hsv2rgb((unsigned char)((unsigned int)HUEMAX * i / LED_NUM), SATMAX, VALMAX, rgb_array[i]);
	}
}
 
/*	придумать схему плавного рандомного перелива цветов
 *	вероятно для этого придется ввести от 2 до 5 опорных точек,
 *	между которыми и будет происходит перелив. Если точки оказываются
 *	слишком близко - они сливаются. каждая точка получает случайные 
 *	приращения цветов(приращения,а не абсолютные значения, для 
 *	плавности изменения цвета) точка также должна перемещаться, причем,
 *	вероятно, координату её лучше задать float для плавного перемещения.
 *
 * 	или плавный перелив просчитывается через приращения rgb координатах
 */