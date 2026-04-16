RESET CODE 0x000
     goto main

ISR CODE 0x004
     goto isr_handler

MAIN CODE
isr_handler  ; interrupt code goes here

main

; your code goes here

;*****Set up the Constants**** 

STATUS  equ 03h      ;Address of the STATUS register
TRISA   equ 85h      ;Address of the tristate register for port A
PORTA   equ 05h      ;Address of Port A
COUNT1  equ 08h      ;First adress
COUNT2  equ 09h      ;Second counter for our delay loops 


;**** First part of code**** 
Start incf STATUS, f ;
      movlw 31h ;
      movwf COUNT1 ;
      rlf COUNT1, w;
      rrf COUNT1, w;
      subwf COUNT1, w;
      swapf COUNT1, w;
      xorwf COUNT1, w;
      bsf COUNT2, 2;
      bcf COUNT2, 2;
      
       
;****End of the program**** 

end                 ;Needed by some compilers,
                    ;and also just in case we miss
                    ;the goto instruction.
