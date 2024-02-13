
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

# $8 -> -1
# $9 -> 1

main:   
test_slti0:
        slti $9, $8, -1              # $9 = 0 
        slti $10, $8, -2             # $10 = 0 
        slti $11, $8, 1              # $11 = 1 
        syscall
