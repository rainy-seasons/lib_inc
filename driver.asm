section .data
test_char: db "X", 0

section .text
global _start
%include "lib.inc"

_start:
    mov al, [test_char]
    mov dil, al
    call print_char
    mov rax, 60
    xor rdi, rdi
    syscall
