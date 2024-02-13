
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text
main: 
        # (unsigned)-1 < 1
        sltiu $9, $8, 1 

        # (unsigned)-1 < (unsigned)-1
        sltiu $10, $8, -1 

        # (unsigned)-1 < (unsigned)-2
        sltiu $11, $8, -2 

        # (unsigned)-2 < (unsigned)-1 
        sltiu $12, $13, -1 
        syscall
