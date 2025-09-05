section .text

string_length:
	xor rax, rax
.loop:
							          ; rdi -> pointer to the string
	cmp byte [rdi + rax], 0           ; check if character is null terminator
	je .end
	inc rax                           ; increment rax as the count
	jmp .loop
.end:
	ret

print_string:
	push rdi                          ; copy the string pointer to the stack
	call string_length
	mov rdx, rax                      ; put the length of the string into rdx (# of bytes to write)
	pop rsi                           ; pop the string pointer from the stack into rsi
	mov rax, 1                        ; write syscall
	mov rdi, 1                        ; stdout
	syscall
	ret

print_char:
	sub rsp, 2                        ; reserve 2 bytes on the stack
	mov byte [rsp], dil               ; store the char
	mov byte [rsp+1], 0               ; store the null terminator
	mov rdi, rsp                      ; pointer to the 2-bytes
	call print_string
	add rsp, 2                        ; clean up the stack
	ret

print_newline:
	mov dil, 0xA
	call print_char
    ret


print_uint:
	mov rax, rdi
	mov rdi, rsp                      ; buffer
	push 0                            ; write null terminator (where string will end)
	sub rsp, 16                       ; reserve 16 bytes
	dec rdi                           ; move rdi one byte back, so the first digit will be written just before the null terminator
	mov r8, 10                        ; 10 is the divisor to convert to decimal

.loop:
	xor rdx, rdx                      ; the high 64 bits of the dividend should be 0. we have a single 64-bit num in rax.
	div r8                            ; divide rax by 10. quotient -> rax. remainder -> rdx.
	or dl, 0x30                       ; convert numeric value into ascii character
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
	mov al, [rdi]                     ; load byte from first string
	mov bl, [rsi]                     ; load byte from second string
	cmp al, bl
	jne .not_equal                    ; if not equal return 0
	test al, al                       ; check for null terminator
	je .equal                         ; if both null, strings are equal
	inc rdi                           ; move to next byte
	inc rsi
	jmp .loop
.not_equal:
	xor rax, rax                      ; return 0
	ret
.equal:
	mov rax, 1                        ; return 1
	ret


read_char:
	push 0                            ; allocate 8 bytes on the stack and init with 0
	xor rax, rax                      ; syscall read (0)
	xor rdi, rdi                      ; fd -> 0 (stdin)
	mov rsi, rsp                      ; buffer = rsp
	mov rdx, 1                        ; read 1 byte
	syscall
	pop rax                           ; pop the value from the buffer back into rax
	ret

read_word:
	push r14                          ; save r14 and r15 (callee-saved registers)
	push r15
	xor r14, r14                      ; r14 -> 0 -> counter (length of word read so far)
	mov r15, rsi                      ; rsi -> buffer length
	dec r15                           ; r15 -> buffer length - 1 (leave space for null terminator)

.A:
	                                  ; if it's a space, \n, \r, or \t then we loop again to skip it
	                                  ; if it's eof, we jump to .C to finish
	                                  ; otherwise, it's a non-whitespace character (start of the word)
	push rdi
	call read_char                    ; read one char into al
	pop rdi
	cmp al, ' '                       ; space?
	je .A
	cmp al, 10                        ; newline?
	je .A
	cmp al, 13                        ; carriage return?
	je .A
	cmp al, 9                         ; tab?
	je .A
	test al, al                       ; eof (read_char returned 0) ?
	jz .C
.B:
	mov byte [rdi + r14], al          ; store the char into buffer[r14]
	inc r14                           ; increase word length

	                                  ; again checking for a space, \n, \r, or \t
	push rdi
	call read_char                    ; get next char
	pop rdi
	cmp al, ' '
	je .C
	cmp al, 10
	je .C
	cmp al, 13
	je .C
	cmp al, 9
	je .C
	test al, al
	jz .C
	cmp r14, r15                      ; did we hit max length? (buffer full)
	je .D

	jmp .B
.C:
	mov byte [rdi + r14], 0           ; null-terminate string
	mov rax, rdi                      ; return buffer pointer in rax
	mov rdx, r14                      ; return length in rdx
	pop r15
	pop r14
	ret
.D:
	xor rax, rax                      ; return 0 = failure
	pop r15
	pop r14
	ret

	                                  ; rdi = source
	                                  ; rsi = dest
	                                  ; rdx = dest length

parse_uint:
	mov r8, 10
	xor rax, rax                      ; rax -> number
	xor rcx, rcx                      ; rcx -> index
.loop:
	movzx r9, byte [rdi + rcx]        ; move next character into r9
	cmp r9b, '0'                      ; checking if the number is a digit '0-9'
	jb .end
	cmp r9b, '9'
	ja .end
	xor rdx, rdx                      ; clear rdx because mul produces a 128-bit result in rdx:rax
	mul r8                            ; multiply rax by 10, store in rax
	and r9b, 0x0f                     ; convert ascii digit into its numeric value
	add rax, r9
	inc rcx                           ; move to the next character
	jmp .loop                         ; repeat
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
	inc rdi                           ; move string pointer past the '-' character
	call parse_uint                   ; call parse_uint to get the magnitude
	neg rax                           ; negate rax to make the number negative
	test rdx, rdx                     ; check if any digits were actually parsed (rdx = len)
	jz .error                         ; if rdx = 0 -> error. string was just '-'

	inc rdx                           ; include the minus sign in the length (rdx++)
	ret                               ; return with rax = signed number

	.error:
	xor rax, rax
	ret


string_copy:
	                                  ; save rdi, rsi, and rdx on the stack beccause they will be written to for string_length
	push rdi
	push rsi
	push rdx
	call string_length                ; get source string length in rax
	                                  ; restore original values
	pop rdx                           ; size
	pop rsi                           ; dest buffer
	pop rdi                           ; source pointer

	cmp rax, rdx                      ; if rax >= rdx -> error (not enough room to copy the string and null terminator)
	jae .too_long                     ; we also need to store null-terminator

	push rsi                          ; save dest pointer for return

.loop:
	mov dl, byte[rdi]                 ; load one byte from source
	mov byte[rsi], dl                 ; store it into destination
	inc rdi                           ; advance source pointer
	inc rsi                           ; advance dest pointer
	test dl, dl                       ; was the byte zero? (null-terminator)
	jnz .loop                         ; if not, continue

	pop rax                           ; dest pointer
	ret

.too_long:
	xor rax, rax
	ret
