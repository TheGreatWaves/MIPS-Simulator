
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL  

# This test assumes that all jump and branching instruction works.

.text

main:   
test_bltzal_take:
	bltzal $3, bltzal_take_1
        addi $15, $0, -1
        syscall
bltzal_take_1:
	addi $8, $ra, 0 # record
	bltzal $4, error
	addi $9, $ra, 0 # record
        syscall
error:
        addi $15, $0, -1
        syscall
