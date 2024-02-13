
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

# Expected $8  -> 0
# Expected $9  -> -1
# Expected $10 -> 100

main:   
        # Is zero, don't jump
        bgtz $8, inf

        # Less than zero, don't jump
	bgtz $9, inf

        # Take it.
 	bgtz $10, jump_over_1
        j inf
jump_over_1:
        j exit
inf:
       addi $15, $0, -1
       j exit
exit:
        syscall
