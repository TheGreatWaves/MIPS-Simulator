
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        sub $10, $8, $9
        sub $7, $0, $9
        sub $6, $9, $0
        syscall
