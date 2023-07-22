; .text
.globl main

main:
    addi $t0, $zero, 5      # $t0 -> 5
    addi $t1, $zero, 10      # $t0 -> 5
    sub $t0, $t0, $t1        # $t0 -> 1
    bgtz $t0, calculate_sum # It should take the branch, or it will get inf loop

inf:
    j inf                   # infinite loop

exit:
    syscall

calculate_sum:
    add $v0, $zero, 10      # $v0 -> 10
    j exit                  # Jump to exit
