
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        xor $4, $8, $8 # 00
        xor $5, $8, $9 # 01
        xor $6, $9, $8 # 10
        xor $7, $9, $9 # 11
        syscall
