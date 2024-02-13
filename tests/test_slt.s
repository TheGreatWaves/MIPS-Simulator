
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
test_slti0:
        slt $9, $8, $4              # $9 = 0 
        slt $10, $8, $5             # $10 = 0 
        slt $11, $8, $6              # $11 = 1 
        syscall
