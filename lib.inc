section .text

string_length:
	xor rax, rax
	.loop:
		cmp byte [rdi + rax], 0 ; check if character is null terminator
		je .end
		inc rax 
		jmp .loop
	.end:
		ret

print_string:
	push rdi     ; copy the string pointer to the stack
	call string_length
	mov rdx, rax ; put the length of the string into rdx
	pop rsi      ; pop the string pointer from the stack into rsi
	mov rax, 1   ; write syscall
	mov rdi, 1   ; stdout
	syscall
	ret

print_char:
    xor rax, rax
    ret

print_newline:
    xor rax, rax
    ret


print_uint:
    xor rax, rax
    ret


print_int:
    xor rax, rax
    ret

string_equals:
    xor rax, rax
    ret


read_char:
    xor rax, rax
    ret 

read_word:
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_uint:
    xor rax, rax
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_int:
    xor rax, rax
    ret 


string_copy:
    ret
