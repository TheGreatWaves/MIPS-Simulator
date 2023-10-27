
# BLEZ BGTZ
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        addi $t0,  $zero, 35          # t0
        addi $t1,  $zero, 1           # t0
        sllv $t3,  $t1  , $t0         # t3 = 8
        addi $t3,  $t3  , 2
        addi $t2,  $zero, 10
        bne  $t2,  $t3  , inf
done:
        j exit
inf:
        addi $t7, $t7, -1
        j exit
exit:
        addiu $v0, $zero, 0xa
        syscall
