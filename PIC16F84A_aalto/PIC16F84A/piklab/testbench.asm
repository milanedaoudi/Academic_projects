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
Start  movlw    03h     ;value 3 to 0x08
       movwf    COUNT1  ;transfer
       movlw    0Eh     ;value 14 to W
       addwf    COUNT1, w  ; add count1(3) and w(14) and put it in W
       movwf    COUNT1     ;move W(17) to COUNT1
       andwf    COUNT1, w   ;and between W=17 and COUNT1=17
       comf     COUNT1, f ;complement of COUNT1 and move it to W
       decf     COUNT1, w   ; decrement COUNT1 and move it to W
       incf     COUNT2, f  ; increment COUNT2 and move it to COUNT2
       iorwf   COUNT2,  w  ;  or between W and COUNT2
       clrf    COUNT1 ; empty COUNT1
       movf   COUNT1, w ; verify COUNT 1 is empty
       movf  COUNT2, w; move count2 to W
       clrw   ;
       
       
;****End of the program**** 

end                 ;Needed by some compilers,
                    ;and also just in case we miss
                    ;the goto instruction.

