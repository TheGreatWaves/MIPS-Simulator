; .text
.globl main

main:
    lui $ra, 0x0040             # Load the upper 16 bits of inf address
    ori $ra, $ra, 0x0010        # Load the lower 16 bits of inf address
    bltzal $t0, calculate_sum   # jump and link the line below
inf:
    j inf                       # infinite loop

exit:
    syscall
    j exit

calculate_sum:
    add $v0, $zero, 10          # $v0 -> 10
    j exit                       # Jump to link
