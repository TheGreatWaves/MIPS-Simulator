; .text
.globl main

main:
    addi $t0, $zero, 5   # $t0 -> 5
    addi $t1, $zero, 2   # $t1 -> 2
    sub $t0, $t0, $t1    # $t0 -> 3
    bgez $t0, calculate_sum

inf:
    j inf # infinite loop

exit:
    syscall

calculate_sum:
    add $v0, $zero, 10  # $v0 -> 10
    j exit             # Jump to exit
