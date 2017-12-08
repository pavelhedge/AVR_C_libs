
/*
 * pow_char.S
 *
 * Created: 28.09.2017 13:55:53
 *  Author: pavelhedge
 *
 *	Функция возведения в степень
 * 
 *	Перемножает число на себя столько раз,
 *	сколько степеней.
 */ 

#define num		r24
#define exp		r22
#define zero	r18
#define temp1	r19
#define temp2	r20
#define temp3	r21

.global pow_char

pow_char:
	subi exp, 1
	brlo exp_zero
	breq exp_one
	push r0
	clr zero
	mov r0, num
exp_start:
	mul r0, num
	cpse r1, zero
	rjmp exp_int
	dec exp
	brne exp_start
exp_end:
	movw num, r0
	pop r0
	clr r1
	ret

exp_int:
	dec exp
	breq exp_end
	mov temp1, r1
	mul r0, num
	movw temp2, r0
	mul temp1, num
	cpse r1, zero
	rjmp exp_err
	add r1, temp3
	adc r0, temp2
	brcs exp_err
	rjmp exp_int 
	 
exp_err:
	pop r0
	clr r1
	clr num
	clr r25
	ret
   
exp_zero:
	ldi num, 1
exp_one:
	ret


