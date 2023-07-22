.text
.globl main

main:
    j calculate_sum   # Jump to the calculate_sum function

exit:
    syscall

calculate_sum:
    addi $t0, $zero, 5 # $t0 -> 5
    addi $t1, $zero, 5 # $t1 -> 5
    add $v0, $t0, $t1  # $v0 -> $t0 + $t1
    j exit             # Jump to exit


