USART_RXC:
/* Посылка начинается со стартбайта, потом байт с номером светодиода в ленте, потом массив данных, по три байта на диод (GRB).  При превышении адреса буфера отправляет ошибку. завершает посылку байт окончания. */
     tst cntr_uart
     breq hello_byte

     cpi uart_cntr, 1
     breq adrs_byte

     in temp, UDR
     cpi temp, end_of_line
     breq last_byte

     ldi temp2, high (RGB_buffer + RGB_buffer_size)
     cpi YL, low (RGB_buffer + RGB_buffer_size)
     cpc YH, temp2
     brsh send_msg_buff_over
     st Y+, temp
reti

     hello_byte:
          in temp, UDR
          cpi temp, start_byte
          breq start_conversion
          reti
          start_conversion:
          inc cntr_uart
reti

     adrs_byte:  
          in temp, UDR
          cpi temp, 60
          brsh send_msg_adrs_over
          mov temp2, temp
          add temp, temp2
          add temp, temp2
          add temp, temp2
          clr temp2
          LDA YY, RGB_buffer
          add YL, temp
          adc YH, temp2
          inc cntr_uart
reti

     last_byte:
          sbiw Y, RGB_buffer
          tst YH
          brne send_msg_buff_over
          clr cntr_uart
          rcall RGB_reload
reti
     
     send_msg_adrs_over:
     msg_adrs_over: .db 'Address exsceeded', end_of_line
          cbi USCRB, UDRIE
          LDA ZZ, (msg_adrs_over *2)
          rcall USART_DRE
reti          

      send_msg_buff_over:
      msg_buff_over: .db 'Reached end of buffer', end_of_line
          cbi USCRB, UDRIE
          LDA ZZ, (msg_buff_over *2)
          rcall USART_DRE
reti          


USART_DRE:
/* Отправка сообщения, записаного в памяти программ. */ 
     tst cntr_uart                    ; На тот случай, если был 
     breq send_last_byte     ; если отправить один байт

     lpm temp, Z+
     out UDR, temp
     cpi temp, end_of_line
     breq send_last_byte
reti
     send_last_byte:
          cbi USCRB, UDRIE
          clr cntr_uart
reti
     

RGB_buffer_shift:
/* Сдвигает буфер на одно значение по кругу и выводит в ленту*/
     LDA XX, RGB_buffer
     ldi cntr_buff, RGB_buffer_size
     ld r0, X+
     ld r1, X+
     ld r2, X+

     RGB_buffer_shift_cycle:
          ld temp1, X+
          ld temp2, X+
          ld temp3, X+
          sbiw XL, 6
          st X+, temp1
          st X+, temp2
          st X+, temp3
          dec cntr_buff
          breq RGB_buffer_shift_last
          adiw XL, 3
          rjmp RGB_buffer_shift_cycle

     RGB_buffer_shift_last:
          st X+, r0
          st X+, r1
          st X+, r3
          rcall RGB_reload
reti