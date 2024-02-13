
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
test_andi_00:
        andi $10, $8, 0
        andi $11, $8, 1
        andi $12, $9, 1
        andi $13, $9, 0
        andi $14, $15, 1365
        syscall
