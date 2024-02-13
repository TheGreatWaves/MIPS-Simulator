
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        # (unsigned)-1 < 1
        sltu $9, $8, $4 

        # (unsigned)-1 < (unsigned)-1
        sltu $10, $8, $5               

        # (unsigned)-1 < (unsigned)-2
        sltu $11, $8, $6              

        # (unsigned)-2 < (unsigned)-1 
        sltu $12, $13, $5               
        syscall
