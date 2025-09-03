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

def main():
    print('test_string_length("hello")')
    print(test_string_length("hello"))
    print('test_string_length("")')
    print(test_string_length(""))

if __name__ == "__main__":
    main()
