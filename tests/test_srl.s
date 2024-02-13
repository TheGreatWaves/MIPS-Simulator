
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        srl $9, $8, 2               # $9 = 8
        srl $10, $8, 3               # $10 = 4
        srl $11, $8, 4               # $11 = 2
        syscall
