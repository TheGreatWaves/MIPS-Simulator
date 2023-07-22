; .text
.globl main

main:
    addi $t0, $zero, 1234
    sra $t1, $t0, 3      # v0 = ???
exit:
    addi $v0, $zero, 10    
    syscall
