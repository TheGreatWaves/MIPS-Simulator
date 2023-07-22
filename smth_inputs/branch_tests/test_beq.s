.text
.globl main

main:
    addi $t0, $zero, 5 # $t0 -> 5
    addi $t1, $zero, 5 # $t1 -> 5
    beq $t1, $t0, calculate_sum

exit:
    syscall

calculate_sum:
    add $v0, $t0, $t1  # $v0 -> $t0 + $t1
    j exit             # Jump to exit



