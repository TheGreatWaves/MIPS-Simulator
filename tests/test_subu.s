
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        subu $10, $8, $9
        subu $7, $0, $9
        subu $6, $9, $0
        syscall
