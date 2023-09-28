

# LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL  

# This test assumes that all jump and branching instruction works.

.text

main:   
        j start_test       # 0
reset:
        addi $t0, $zero, 0 # 4
        addi $t1, $zero, 0 # 8
        addi $t2, $zero, 0 # c
        addi $t3, $zero, 0 # 10
        addi $t4, $zero, 0 # 14
        addi $t6, $t6, 1   # 18
        mthi $zero         # 1c
        mtlo $zero         # 20
        jr $ra             # 24
start_test:
test_bgezal_take_1:
	add $t1, $zero, $ra       # Record the return address, write 0. 2c
	bgezal $t0, bgezal_take_1 # Should return back here right away. 30
	beq $ra, $t1, inf         # Should have changed 2c
	jal reset		  # 34
	j bgezal_take_done_1      # 38
bgezal_take_1:
	jr $ra                    # 3c
bgezal_take_done_1:
test_bgezal_take_2:
	addi $t1, $zero, 69       # 40
	add $t1, $zero, $ra       # Record the return address.
	bgezal $t0, bgezal_take_2 # Should return back here right away.
	beq $ra, $t1, inf         # Should have changed
	jal reset
	j bgezal_take_done_2
bgezal_take_2:
	jr $ra
bgezal_take_done_2:
test_bgezal_no_take:
	addi $t0, $zero, -4
	add $t1, $zero, $ra       # Record the return address.
	bgezal $t0, test_bgezal_not_taken
	beq $ra, $t1, inf         # Should not have changed
	j test_bgezal_done
test_bgezal_not_taken:
	jr $ra
test_bgezal_done:
	jal reset
done:
        j exit
inf:
        addi $t7, $t7, -1
        j exit
exit:
        addiu $v0, $zero, 0xa
        syscall
