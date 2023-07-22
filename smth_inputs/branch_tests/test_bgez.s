; .text
.globl main

main:
    addi $t0, $zero, 5 # $t0 -> 5
    addi $t1, $zero, 5 # $t1 -> 5
    sub $t0, $t0, $t1    # $t0 -> 0
    bgez $t0, calculate_sum

inf:
    j inf # infinite loop

exit:
    syscall

calculate_sum:
    add $v0, $zero, 10  # $v0 -> 10
    j exit             # Jump to exit
