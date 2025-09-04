section .text

string_length:
	xor rax, rax
.loop:
							      ; rdi -> pointer to the string
	cmp byte [rdi + rax], 0       ; check if character is null terminator
	je .end
	inc rax                       ; increment rax as the count
	jmp .loop
.end:
	ret

print_string:
	push rdi                      ; copy the string pointer to the stack
	call string_length
	mov rdx, rax                  ; put the length of the string into rdx (# of bytes to write)
	pop rsi                       ; pop the string pointer from the stack into rsi
	mov rax, 1                    ; write syscall
	mov rdi, 1                    ; stdout
	syscall
	ret

print_char:
	sub rsp, 2                    ; reserve 2 bytes on the stack
	mov byte [rsp], dil           ; store the char
	mov byte [rsp+1], 0           ; store the null terminator
	mov rdi, rsp                  ; pointer to the 2-bytes
	call print_string
	add rsp, 2                    ; clean up the stack
	ret

print_newline:
	mov dil, 0xA
	call print_char
    ret


print_uint:
	mov rax, rdi
	mov rdi, rsp                  ; buffer
	push 0                        ; write null terminator (where string will end)
	sub rsp, 16                   ; reserve 16 bytes
	dec rdi                       ; move rdi one byte back, so the first digit will be written just before the null terminator
	mov r8, 10                    ; 10 is the divisor to convert to decimal

.loop:
	xor rdx, rdx                  ; the high 64 bits of the dividend should be 0. we have a single 64-bit num in rax.
	div r8                        ; divide rax by 10. quotient -> rax. remainder -> rdx.
	or dl, 0x30                   ; convert numeric value into ascii character
	dec rdi
	mov [rdi], dl
	test rax, rax
	jnz .loop

	call print_string
	add rsp, 24
	ret


print_int:
	test rdi, rdi
	jns print_uint
	push rdi
	mov rdi, '-'
	call print_char
	pop rdi
	neg rdi
	jmp print_uint


string_equals:
	xor rax, rax
.loop:
	mov al, [rdi] ; load byte from first string
	mov bl, [rsi] ; load byte from second string
	cmp al, bl
	jne .not_equal ; if not equal return 0
	test al, al ; check for null terminator
	je .equal ; if both null, strings are equal
	inc rdi ; move to next byte
	inc rsi
	jmp .loop
.not_equal:
	xor rax, rax ; return 0
	ret
.equal:
	mov rax, 1 ; return 1
	ret


read_char:
    xor rax, rax
    ret

read_word:
    ret

                                  ; rdi points to a string
                                  ; returns rax: number, rdx : length
parse_uint:
	mov r8, 10
	xor rax, rax                  ; rax -> number
	xor rcx, rcx                  ; rcx -> index
.loop:
	movzx r9, byte [rdi + rcx]    ; move next character into r9
	cmp r9b, '0'                  ; checking if the number is a digit '0-9'
	jb .end
	cmp r9b, '9'
	ja .end
	xor rdx, rdx                  ; clear rdx because mul produces a 128-bit result in rdx:rax
	mul r8                        ; multiply rax by 10, store in rax
	and r9b, 0x0f                 ; convert ascii digit into its numeric value
	add rax, r9
	inc rcx                       ; move to the next character
	jmp .loop                     ; repeat
	.end:
	mov rdx, rcx
	ret

parse_int:
	                              ; check the first character. if it's '-', jump to handle negative number. otherwise call parse_uint
	mov al, byte [rdi]
	cmp al, '-'
	je .signed
	jmp parse_uint
.signed:
	inc rdi                       ; move string pointer past the '-' character
	call parse_uint               ; call parse_uint to get the magnitude
	neg rax                       ; negate rax to make the number negative
	test rdx, rdx                 ; check if any digits were actually parsed (rdx = len)
	jz .error                     ; if rdx = 0 -> error. string was just '-'

	inc rdx                       ; include the minus sign in the length (rdx++)
	ret                           ; return with rax = signed number

	.error:
	xor rax, rax
	ret


string_copy:
    ret
