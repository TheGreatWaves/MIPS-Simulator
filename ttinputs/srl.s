; .text
.globl main

main:
    addi $t0, $zero, 32  # t0 = 1
    srl $v0, $t0, 2    # v0 = 8
    addi $v0, $v0, 2   # jumps to exit if t1 is 0
    syscall
