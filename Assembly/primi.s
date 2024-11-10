@; input:  r0 intero unsigned >1
@; output: r0=0 se composto, l'input se è primo 

    .data
errmsg: .string "Input non valido"

    .text
    .global primi
    .type primi, %function

primi:
    cmp r0, #2 // r0 == 2?
    blt end // mi fermo
    cmp r0, #2 // se r0 è uguale a 2 ritorno al chiamante
    mov pc, lr
    and r1, r0, #1 // r1 = r0 e 1 = r0%2
    cmp r1, #0 // è pari?
    bne odd // è dispari il ciclo continua
nonprimo:
    mov r0, #0
odd:
    mov r1, #3 @ for (int i = 3; i < N; i++)
loop:
    mul r3, r1, r1
    cmp r3, r0 @ r1 * r1 > 0
    bhi end
    @; calcolo resto r3 = r0%r1 per sottrazioni successive (inefficiente)
    mov r3, r0
resto1:
    cmp r3, r1
    blo resto2
    sub r3, r3, r1
    b resto1
resto2:
    cmp r3, #0 @ non primo
    beq nonprimo
    add r1, r1, #2 @ ri += 2 aggiorna variabile loop
    b loop
end:
    mov pc, lr

@ stampa msg errore
stop:
    ldr, r0=errmsg
    bl perror
    mov r0, #33 @ non salvo lr perchè non ritorna
    b exit


@; necessario per arm-linux-gnueabihf-gcc per evitare il warning
@; missing .note.GNU-stack section implies executable stack
.section .note.GNU-stack,"",%progbits
    
