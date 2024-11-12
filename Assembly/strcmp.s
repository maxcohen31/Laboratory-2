@; input:  
@;	r0, r1: array caratteri 0-terminati
@; output: 
@;	r0: valore < > ==0 a seconda che la stringa r0 sia 
@;	lessicograficamente minore maggiore o uguale a r1

    .data
    .text
    .global armcmp
    .type armcmp, %function

armcmp:
    cmp r0, #0 @; stringa nulla?
    moveq r0, #11
    beq exit
    cmp r1, #0
    moveq r1, #12
    beq exit
    mov r2, #0 @; i = 0

loop:
    ldrb r3, [r0, r2] @; carica stringa[i] -> s[i]
    ldrb r12 [r1, r2] @; carica t[i]
    cmp r3, r12
    subne r0, r3, r12 @; se s[i] != t[i] allora s[i] - t[i]
    mov pc, lr // return 0
    cmp r0, #0 @; s[i] == 0
    moveq r0, #0
    moveq pc, lr
    add r2, r2, #1 @; i++
    b loop


@; necessario per arm-linux-gnueabihf-gcc per evitare il warning
@; missing .note.GNU-stack section implies executable stack
.section .note.GNU-stack,"",%progbits
