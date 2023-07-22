; .text
.globl main

main:
    blez $t0, calculate_sum

inf:
    j inf # infinite loop

exit:
    syscall

calculate_sum:
    add $v0, $zero, 10  # $v0 -> 10
    j exit             # Jump to exit
