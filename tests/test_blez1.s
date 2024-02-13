
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

# Expected $8  -> 100
# Expected $9  -> 0
# Expected $10 -> -1

main:   
        blez $8, inf
	blez $9, jump_over_1
	j inf 				# jump over this
jump_over_1:
# 	blez $10, jump_over_2
# 	j inf 				# jump over this
# jump_over_2:
        j exit
inf:
       addi $15, $0, -1
       j exit
exit:
        syscall
