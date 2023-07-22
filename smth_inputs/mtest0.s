; .text
.globl main

main:
    addi $t0, $zero, -623     # t0 = 2
    addi $t1, $zero, 2    # t1 = idk man in lazy
    mult $t0, $t1          # LO = 0xffffffff, HI = 1
    mflo $t3
    mfhi $t2
exit:
    addi $v0, $zero, 10
    syscall
