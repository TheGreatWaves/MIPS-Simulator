
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
test_sllv:
        sllv $10,  $9, $8         # $10 = 8
        sllv $13,  $12, $11       # $13 = 4
        syscall
