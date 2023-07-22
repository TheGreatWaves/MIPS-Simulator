; .text
.globl main

main:
    addi $t0, $zero, 0xfffffff0    # t0 = 11110000
    addi $t1, $zero, 0xfffffffe    # t1 = 11111110
    nor  $t2, $t1, $t0             # t2 = 1
    addi $t0, $zero, 2             # t0 = 2
    addi $t1, $zero, 0xffffffff    # t1 = idk man in lazy
    mult $t0, $t1                  # LO = 0xffffffff, HI = 1
    addi $v0, $zero, 10
    syscall
