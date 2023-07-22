; .text
.globl main

main:
    addi $t0, $zero, -1  # t0 = -1
    sltiu $t1, $t0, 1    # t1 = 0 
    beq $t1, $zero, exit   # jumps to exit if t1 is 0
    
inf:
    j inf # infinite loop

exit:
    addi $v0, $zero, 10  # v0 = 10
    syscall
