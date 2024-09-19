GLOBAL divZero
GLOBAL opcode:

section .text
divZero:
    mov rax, 5
    mov rcx, 0
    div rcx
    ret

opcode:
    ud2
    ret