RESET CODE 0x000
    goto    main        ; Jump to main program

ISR CODE 0x004
    goto    isr_handler ; Interrupt vector

MAIN CODE
isr_handler ; interrupt code goes here
;***** Constants *****
STATUS  equ 03h        ; STATUS register
TRISA   equ 85h        ; TRIS A register
PORTA   equ 05h        ; PORT A
COUNT1  equ 08h        ; Counter 1
COUNT2  equ 09h        ; Counter 2

;***** Main Program *****
main

Start:
    ; Demonstrate movlw and addlw
    movlw   0x10        ; Load 0x10 into W
    addlw   0x20        ; Add 0x20 to W (W now 0x30)
    movwf   COUNT1       ; Output result to COUNT1
    
    ; Call a subroutine
    call    math_operations  ; Call math subroutine
    
    ; Demonstrate goto
    goto    main        ; Infinite loop

;***** Subroutines *****
math_operations:
    iorlw   0xF0        ; OR operation
    sublw  0xFA;
    retlw   0xFF        ; Return with value 0xFF in W




end