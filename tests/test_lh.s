

# This test assumes that all jump and branching instruction works.

.text

main:   
	lh $8, 0($3)
	lh $9, 0($4)
	lh $10, 0($5)
	lh $11, 1($4)
	syscall
