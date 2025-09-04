section .text
global _start
%include "lib.inc"

_start:
    call print_newline
    mov rax, 60
    xor rdi, rdi
    syscall
