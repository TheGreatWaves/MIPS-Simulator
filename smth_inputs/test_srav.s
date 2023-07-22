
; .text
.globl main

main:
    addi $t3,  $zero, -13  # t3 -> -13
    addi $t0,  $zero, -100 # t0 -> -100
    addi $t1,  $zero, 35   # t1 -> 35
    srav $t2,  $t0, $t1    # t2 -> (-100) >> 3
    beq  $t2,  $t3, exit   # Jump to exit if $t2 = -13
inf:
    j inf
exit:
    addi $v0, $zero, 0xa
    syscall

