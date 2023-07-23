
# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SRA 

# This test assumes that all jump and branching instruction works.

.text

main:   
        j start_test
reset:
        addi $t0, $zero, 0
        addi $t1, $zero, 0
        addi $t2, $zero, 0
        addi $t3, $zero, 0
        addi $t4, $zero, 0
        addi $t6, $t6, 1
        mthi $zero
        mtlo $zero
        jr $ra
start_test:
test_sra_neg:
	addi $t0, $zero, 3
	addi $t1, $zero, -16
	sra $t2, $t1, $t0
	addi $t0, $zero, -2
	bne $t0, $t2 inf
	jal reset
test_sra_neg_1:
	addi $t0, $zero, 3
	addi $t1, $zero, -69
	sra $t2, $t1, $t0
	addi $t0, $zero, -9
	bne $t0, $t2 inf
	jal reset
test_sra_pos:
	addi $t0, $zero, 3
	addi $t1, $zero, 16
	sra $t2, $t1, $t0
	addi $t0, $zero, 2
	bne $t0, $t2 inf
	jal reset
done:
        j exit
inf:
        addi $t7, $t7, -1
        j exit
exit:
        addiu $v0, $zero, 0xa
        syscall
