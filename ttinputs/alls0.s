; .text
.globl main

main:
    addi $t0, $zero, 8            # t0 = 1000
    srl $t1, $t0, 3               # t1 = 0001
    sll $t2, $t0, 1               # t2 = 10000
    sub $t2, $t2, 15              # t2 = 0001
    addi $t0, $zero, -1           # t0 = 11111111111111111111111111111111
    srl $t3, $t0, 31              # t3 = 1
    sll $t4, $t0, 31              # t4 = 10000000000000000000000000000000
    sub $t4, $t4, 0x80000000
    addi $t4, $t4, 1              # t4 = 1
    addi $t0, $zero, 8            # t0 = 1000
    sra $t5, $t0, 3               # t5 = 1111
    sub $t5, $t5, 14              # t5 = 1
    addi $v0, $zero, 10           
    syscall
