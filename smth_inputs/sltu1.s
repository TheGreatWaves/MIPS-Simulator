; .text
.globl main

main:
    addi $t0, $zero, -1  # t0 = -1
    addi $t2, $zero, -2  # t2 = 1
    sltu $t1, $t0, $t2   # t1 = 1
    beq $t1, $zero, exit   # jumps to exit if t1 is 0
    
inf:
    j inf # infinite loop

exit:
    addi $v0, $zero, 10  # v0 = 10
    syscall
