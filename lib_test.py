import subprocess
import os

def test_string_length(s):
    asm_c = f"""section .data
test_str: db "{s}", 0

section .text
global _start
%include "lib.inc"

_start:
    mov rdi, test_str    ; pointer to the string
    call string_length   ; call the function -- returns length in rax
    mov rdi, rax         ; move the length into rdi for exit
    mov rax, 60          ; exit syscall #
    syscall
"""
    return build(asm_c, capture_stdout=False)

def test_print_string(s):
    asm_c = f"""section .data
test_str: db "{s}", 0

section .text
global _start
%include "lib.inc"

_start:
    mov rdi, test_str
    call print_string
    mov rax, 60
    xor rdi, rdi
    syscall
"""
    return build(asm_c)

def test_print_char(c):
    asm_c = f"""section .data
test_char: db "{c}", 0

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
"""
    return build(asm_c)

def test_print_newline():
    asm_c = f"""section .text
global _start
%include "lib.inc"

_start:
    call print_newline
    mov rax, 60
    xor rdi, rdi
    syscall
"""
    output = build(asm_c)

    if output == "\n":
        return "ok"
    else:
        return f"Fail: captured {repr(output)}"

def test_print_uint(n):
    asm_c = f"""section .text
global _start
%include "lib.inc"

_start:
    mov rdi, {n} ; num to print
    call print_uint
    mov rax, 60
    xor rdi, rdi
    syscall
"""
    return build(asm_c)

def test_parse_uint(s):
    asm_c = f"""section .data
test_str: db "{s}", 0

section .text
global _start
%include "lib.inc"

_start:
    mov rdi, test_str    ; pointer to string
    call parse_uint      ; rax = number, rdx = length
    mov rdi, rax         ; exit code = parsed number
    mov rax, 60          ; exit syscall
    syscall
"""
    return build(asm_c, capture_stdout=False)

def test_string_equals(s1, s2):
    asm_c = f"""
str1: db "{s1}", 0
str2: db "{s2}", 0

section .text
global _start
%include "lib.inc"

_start:
    mov rdi, str1
    mov rsi, str2
    call string_equals ; returns 1 if equal, 0 otherwise
    add al, '0' ; convert to ascii '0' or '1'
    mov dil, al
    call print_char
    mov rax, 60
    xor rdi, rdi
    syscall
"""
    res = build(asm_c)
    if res == '1':
        return "TRUE"
    else:
        return "FALSE"

def test_read_char(s):
    asm_c = """section .text
global _start
%include "lib.inc"

_start:
    call read_char
    mov rdi, rax    ; char result
    test rax, rax   ; if 0 (EOF), skip printing
    jz .done
    call print_char
.done:
    mov rax, 60
    xor rdi, rdi
    syscall
"""
    return build(asm_c, input_str=s)

def test_string_copy(s, bufsize):
    asm_c = f"""section .data
src_str: db "{s}", 0

section .bss
dest_str: resb {bufsize}

section .text
global _start
%include "lib.inc"

_start:
    mov rdi, src_str
    mov rsi, dest_str
    mov rdx, {bufsize}
    call string_copy
    mov rdi, rax
    test rax, rax
    jz .fail
    call print_string
    jmp .exit
.fail:
    mov rdi, failmsg
    call print_string
.exit:
    mov rax, 60
    xor rdi, rdi
    syscall

section .data
failmsg: db "FAIL", 0
"""
    return build(asm_c)

def test_read_word(s, bufsize=16):
    asm_c = f"""section .bss
dest_str: resb {bufsize}

section .text
global _start
%include "lib.inc"

_start:
    mov rdi, dest_str
    mov rsi, {bufsize}
    call read_word
    mov rdi, rax
    test rax, rax
    jz .fail
    call print_string
    jmp .exit
.fail:
    mov rdi, failmsg
    call print_string
.exit:
    mov rax, 60
    xor rdi, rdi
    syscall

section .data
failmsg: db "FAIL", 0
"""
    return build(asm_c, input_str=s)


def build(asm, capture_stdout=True, input_str=None):
    with open("driver.asm", "w") as f:
        f.write(asm)
    subprocess.run(["nasm", "-f", "elf64", "driver.asm", "-o", "driver.o"], check=True)
    subprocess.run(["ld", "driver.o", "-o", "driver", "-e", "_start"], check=True)
    os.chmod("driver", 0o755)

    res = subprocess.run(
        ["./driver"],
        input=input_str.encode() if input_str is not None else None,
        stdout=subprocess.PIPE if capture_stdout else None
    )
    return res.stdout.decode() if capture_stdout else res.returncode

def main():
    print('test_string_length("hello")')
    print(test_string_length("hello"))
    print('test_string_length("")')
    print(test_string_length(""))

    print('test_print_string("hello")')
    print(test_print_string("hello"))
    print('test_print_string("")')
    print(test_print_string(""))

    print('test_print_char("A")')
    print(test_print_char("A"))
    print('test_print_char("X")')
    print(test_print_char("X"))

    print("test_print_newline()... " + test_print_newline())

    print("test_print_uint(5) -> " + test_print_uint(5))
    print("test_print_uint(42) -> " + test_print_uint(42))

    print(f"test_parse_uint('45') -> {test_parse_uint('45')}")

    print('test_string_equals("hello", "hello")')
    print(test_string_equals("hello", "hello"))
    print('test_string_equals("hello", "world")')
    print(test_string_equals("hello", "world"))
 
    print('test_read_char("A") -> ')
    print(test_read_char("A"))
    print('test_read_char("") (EOF) -> ')
    print(test_read_char(""))

    print('test_string_copy("hello", 10) -> ') 
    print(test_string_copy("hello", 10))
    print('test_string_copy("toolong", 5) -> ') 
    print(test_string_copy("toolong", 5))

    print('test_read_word("hello world") -> ') 
    print(test_read_word("hello world"))
    print('test_read_word("   spaced") -> ') 
    print(test_read_word("   spaced"))
    print('test_read_word("") -> ') 
    print(test_read_word(""))


if __name__ == "__main__":
    main()
