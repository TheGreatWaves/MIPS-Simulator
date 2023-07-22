.text
.globl main

main:
    
    jal calculate_sum    # Perform a function call
    syscall              # Should halt if everything goes well

calculate_sum:
    addi $t0, $zero, 5   # $t0 -> 5
    addi $t1, $zero, 5   # $t1 -> 5
    add $v0, $t0, $t1    # $v0 -> $t0 + $t1
    jr $t4               # Return to the calling function
