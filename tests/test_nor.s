

# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        nor $4, $8, $8 # 00
        nor $5, $8, $9 # 01
        nor $6, $9, $8 # 10
        nor $7, $9, $9 # 11
        syscall
