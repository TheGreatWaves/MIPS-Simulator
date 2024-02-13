
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        # Store 0x400004 in $ra
        jalr $3                  # 0
        addi $15, $0, -1 # noop  # 4
        addi $15, $0, -1 # noop  # 8
        addi $15, $0, -1 # noop  # c
        jalr $5, $4              # 10
        addi $15, $0, -1 # noop  # 14
        syscall                  # 18
        addi $15, $0, -1 # noop  # 1c
        jalr $6, $7              # 20
        addi $15, $0, -1 # noop  # 24
