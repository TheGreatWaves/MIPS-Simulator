
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
test_xori_00:
        xori $10, $8, 0
        xori $11, $8, 1
        xori $12, $9, 1
        xori $13, $9, 0
        syscall
