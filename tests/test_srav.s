
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
test_srav:
        srav $10,  $8, $9           # $10 -> (-100) >> 3
        srav $13,  $11, $12         # $13 -> (-32) >> 1
        syscall
