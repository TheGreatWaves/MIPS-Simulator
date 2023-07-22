; # This should halt.
; .text
.globl main

main:
    addi $t1, $zero, 1
    sub $t0, $t0, $t1             # $t0 -> -1
    bltzal $t0, calculate_sum     # jump and link the line below

exit: # infinite loop if $v0 never set
    syscall
    j exit

calculate_sum:
    add $v0, $zero, 10          # $v0 -> 10
    j $ra                       # Jump to link
