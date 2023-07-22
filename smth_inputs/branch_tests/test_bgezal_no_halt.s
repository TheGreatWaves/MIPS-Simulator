
; # This should halt.
; .text
.globl main

main:
    addi $t0, $t0, 1             # $t0 is negative, branch not taken
    sub $t0, $zero, $t0             # $t0 is negative, branch not taken
    bgezal $t0, calculate_sum   # jump and link the line below

exit: # infinite loop if $v0 never set
    syscall
    j exit

calculate_sum:
    add $v0, $zero, 10          # $v0 -> 10
    j $ra                       # Jump to link
