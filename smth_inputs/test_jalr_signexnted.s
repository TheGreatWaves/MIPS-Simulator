.text
.globl main

main:
    lui $t0, 0x0040        # Load the upper 16 bits of calculate_sum address
    ori $t0, $t0, 0x0010   # Load the lower 16 bits of calculate_sum address
    jalr $t4, $t0          # Perform a function call using JALR, link where to jump back in $t4
    addi $v0, $v0, 1       # $v0 -> 9 + 1
    syscall                # Should halt if everything goes well

calculate_sum:
    addi $t0, $zero, 5     # $t0 -> 5
    addi $t1, $zero, 4     # $t1 -> 4
    add $v0, $t0, $t1      # $v0 -> $t0 + $t1 = 9
    jr $t4                 # Return to the calling function via jumping to addr in $t4
