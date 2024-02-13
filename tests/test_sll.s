
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        sll $9, $8, 3               # t2 = 8
        sll $10, $8, 4               # t2 = 8
        syscall
