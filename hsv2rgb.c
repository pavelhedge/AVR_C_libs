/*
 *	Перевод HSV шкалы в RGB для вывода
 *	HSV удобней для расчета цветов
 *	а RGB используется во всех внешних устройствах
 *	
 *	HSV применяется не стандартная (Н 0-360, S 0-100, V 0-100),
 *	а та, которая удобней для обработки в 8 битном режиме
 *
 *	Для граничных случаев - упрощенные методы
 *
 */
 
 #define HUEMAX		180						// 	размер шкалы тонов
 #define SATMAX 	255						//	макс. знач. насыщенности
 #define VALMAX		255						//	макс. знач. яркости
 #define HUE_DIV	30						//	размер сектора цветового тона
 
	
void hsv2rgb (unsigned char hue, unsigned char sat, unsigned char val, unsigned char rgb[3]){
	unsigned char v_min;
	unsigned char v_diff;
	
// в светодиодной ленте порядок GRB !!!
//	Проверки граничных условий:
//	яркость равна нулю - черный
	if (val == 0){
		rgb[0] = 0;
		rgb[1] = 0;
		rgb[2] = 0;
		return;
	}
	
//	насыщенность равна нулю - отенки серого R=G=B=val
	if (sat == 0){
		rgb[0] = val;
		rgb[1] = val;
		rgb[2] = val;
		return;
	}

//	насыщенность максимальна - чистый тон
	if(sat == SATMAX){
		v_min = 0;
	}else{
		v_min = (unsigned char)((unsigned short)(SATMAX - sat) * val / SATMAX);
	}
	v_diff = (unsigned char)((unsigned short)(val - v_min)*(hue % HUE_DIV)/HUE_DIV);
	
	switch(hue/HUE_DIV){		//	Порядок в ленте GRB
		case 0:	rgb[1] = val ;
				rgb[0] = v_min + v_diff;
				rgb[2] = v_min;
						break;
						
		case 1:	rgb[1] = val - v_diff;
				rgb[0] = val;
				rgb[2] = v_min;
						break;
						
		case 2:	rgb[1] = v_min;
				rgb[0] = val;
				rgb[2] = v_min + v_diff;
						break;
						
		case 3:	rgb[1] = v_min ;
				rgb[0] = val - v_diff;
				rgb[2] = val;
						break;

		case 4:	rgb[1] = v_min + v_diff;
				rgb[0] = v_min;
				rgb[2] = val;
						break;
						
		case 5:	rgb[1] = val;
				rgb[0] = v_min;
				rgb[2] = val - v_diff ;
						break;
	}
	
	return;
}