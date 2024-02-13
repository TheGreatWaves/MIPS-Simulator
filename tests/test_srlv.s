
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        srlv $9, $8, $3               # $9 = 8
        srlv $10, $8, $4               # $10 = 4
        srlv $11, $8, $5               # $11 = 2
        syscall
