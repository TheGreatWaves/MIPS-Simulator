# This test assumes that all jump and branching instruction works.

.text

main:   
test_sw:
	sw $8, 0($3)
	sw $9, 4($3)
	sw $10, 0($4) # store -1
        syscall
