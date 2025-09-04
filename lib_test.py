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

    with open("driver.asm", "w") as f:
        f.write(asm_c)

    # assemble
    subprocess.run(["nasm", "-f", "elf64", "driver.asm", "-o", "driver.o"], check=True)

    # link
    subprocess.run(["ld", "driver.o", "-o", "driver", "-e", "_start"], check=True)

    # ensure executable
    os.chmod("driver", 0o755)

    # run and return exit code
    result = subprocess.run(["./driver"])
    return result.returncode

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
    with open("driver.asm", "w") as f:
        f.write(asm_c)

    subprocess.run(["nasm", "-f", "elf64", "driver.asm", "-o", "driver.o"], check=True)
    subprocess.run(["ld", "driver.o", "-o", "driver", "-e", "_start"], check=True)

    res = subprocess.run(["./driver"], stdout=subprocess.PIPE)
    output = res.stdout.decode()
    return output

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
    with open("driver.asm", "w") as f:
        f.write(asm_c)

    subprocess.run(["nasm", "-f", "elf64", "driver.asm", "-o", "driver.o"], check=True)
    subprocess.run(["ld", "driver.o", "-o", "driver", "-e", "_start"], check=True)

    res = subprocess.run(["./driver"], stdout=subprocess.PIPE)
    output = res.stdout.decode()
    return output

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
    with open("driver.asm", "w") as f:
        f.write(asm_c)

    subprocess.run(["nasm", "-f", "elf64", "driver.asm", "-o", "driver.o"], check=True)
    subprocess.run(["ld", "driver.o", "-o", "driver", "-e", "_start"], check=True)

    res = subprocess.run(["./driver"], stdout=subprocess.PIPE)
    output = res.stdout.decode()

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
    with open("driver.asm", "w") as f:
        f.write(asm_c)

    subprocess.run(["nasm", "-f", "elf64", "driver.asm", "-o", "driver.o"], check=True)
    subprocess.run(["ld", "driver.o", "-o", "driver", "-e", "_start"], check=True)

    res = subprocess.run(["./driver"], stdout=subprocess.PIPE)
    output = res.stdout.decode()
    return output

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
    with open("driver.asm", "w") as f:
        f.write(asm_c)

    subprocess.run(["nasm", "-f", "elf64", "driver.asm", "-o", "driver.o"], check=True)
    subprocess.run(["ld", "driver.o", "-o", "driver", "-e", "_start"], check=True)

    result = subprocess.run(["./driver"])
    return result.returncode

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
 


if __name__ == "__main__":
    main()
