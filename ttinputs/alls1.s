; .text
.globl main

main:
    addi $t0, $zero, 1    # t0 = 0001
    addi $t1, $zero, 67   # t1 = 64 + 3 -> so I shift by 3
    sllv $t2, $t0, $t1    # t2 = 1000
    sub $t2, $t2, 7       # t2 = 1
    addi $t0, $zero, 8    # t0 = 1000
    srlv $t3, $t0, $t1    # t3 = 0001 
    srav $t4, $t0, $t1    # t4 = 1111
    sub $t4, $t4, 15      # t4 = 1
    addi $v0, $zero, 10
    syscall
