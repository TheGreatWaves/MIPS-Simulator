; .text
.globl main

main:
    addi $t0,  $zero, 35     # t0
    addi $t1,  $zero, 1      # t0
    sllv $v0,  $t1  , $t0    # v0 = 8
    addi $v0,  $v0  , 2
    addi $t2,  $zero, 10
    beq  $t2,  $v0  , exit
inf:
    j inf
exit:
    syscall

