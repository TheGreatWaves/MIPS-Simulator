

# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL  

# This test assumes that all jump and branching instruction works.

.text

# $3 == -1
# $4 == 1

main:   
	bltz $3, bltz_take_1
error:
        # This should be jumped over.
	addi $15, $0, -1
        syscall
bltz_take_1:
        # We should not take it.
	bltz $4, error
        syscall
