


# This test assumes that all jump and branching instruction works.

.text

main:   
	lhu $8, 0($3)
	lhu $9, 0($4)
	lhu $10, 0($5)
	lhu $11, 1($4)
	syscall
