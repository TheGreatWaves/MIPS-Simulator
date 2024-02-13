
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
test_sra_neg:
	sra $10, $9, $8
	sra $13, $12, $11
	sra $6, $5, $4
        syscall
