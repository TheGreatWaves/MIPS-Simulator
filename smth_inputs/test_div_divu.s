; .text
.globl main

main:
    addi $t0, $zero, -19             # t0 = -19
    addi $t1, $zero, 4               # t1 = 4
    div $t0, $t1                    # LO = 4, HI = 3
    divu $t0, $t1                   # LO = 0xFFF6, HI = no clue
    addi $v0, $zero, 10
    syscall