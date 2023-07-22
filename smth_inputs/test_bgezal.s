; # This should halt.
; .text
.globl main

main:
    bgezal $t0, calculate_sum   # jump and link the line below

exit: # infinite loop if $v0 never set
    syscall
    j exit

calculate_sum:
    add $v0, $zero, 10          # $v0 -> 10
    j $ra                       # Jump to link
