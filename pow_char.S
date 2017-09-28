
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
#define temp	r18
#define temp2	r19
#define temp3	r20
#define temp4	r21

.global pow_char

pow_char:
	subi exp, 1
	brlo exp_zero
	breq exp_one
	push r0
	clr temp
	mov r0, num
exp_start:
	mul r0, num
	cpse r1, temp
	rjmp exp_int
	dec exp
	brne exp_start
exp_end:
	movw num, temp3
	pop r0
	clr r1
	ret

exp_int:
	dec exp
	breq exp_end
	mov temp2, r1
	mul r0, num
	movw temp3, r0
	mul temp2, num
	cpse r1, temp
	rjmp exp_err
	add temp4, r0
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


