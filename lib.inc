section .text

string_length:
	xor rax, rax
	.loop:
		                        ; rdi -> pointer to the string
		cmp byte [rdi + rax], 0 ; check if character is null terminator
		je .end
		inc rax                 ; increment rax as the count
		jmp .loop
	.end:
		ret

print_string:
	push rdi                    ; copy the string pointer to the stack
	call string_length
	mov rdx, rax                ; put the length of the string into rdx (# of bytes to write)
	pop rsi                     ; pop the string pointer from the stack into rsi
	mov rax, 1                  ; write syscall
	mov rdi, 1                  ; stdout
	syscall
	ret

print_char:
	sub rsp, 2                  ; reserve 2 bytes on the stack
	mov byte [rsp], dil         ; store the char
	mov byte [rsp+1], 0         ; store the null terminator
	mov rdi, rsp                ; pointer to the 2-bytes
	call print_string
	add rsp, 2                  ; clean up the stack
	ret

print_newline:
	mov dil, 0xA
	call print_char
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
