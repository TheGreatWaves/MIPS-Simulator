# This test assumes that all jump and branching instruction works.

.text

main:   
	addiu $t0, $zero, 5
	addiu $t1, $zero, 5
        beq $t0, $t1, next
        addiu $t0, $zero, 0                    
        addiu $t1, $zero, 0                    
next:
	beq $t0, $zero, next_next  # not taken
exit:
        addiu $v0, $zero, 0xa
        syscall
next_next:			# not taken
	addiu $t0, $zero, 5
	addiu $t1, $zero, 5
        addiu $v0, $zero, 0xa
        syscall
