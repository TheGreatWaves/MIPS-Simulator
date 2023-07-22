; .text
.globl main

main:
    addi $t0, $zero, 1  # t0 = 1
    sll $v0, $t0, 3    # v0 = 8
    addi $v0, $v0, 2   # jumps to exit if t1 is 0
    syscall
