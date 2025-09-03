section .text

string_length:
	xor rax, rax ; zero out rax
	.loop:
		cmp byte [rdi + rax], 0 ; check if character is null terminator
		je .end
		inc rax ; character is not null terminator, so we increment and loop
		jmp .loop
	.end:
		ret

print_string:
    xor rax, rax
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
