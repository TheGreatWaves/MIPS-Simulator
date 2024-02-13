# This test assumes that all jump and branching instruction works.

.text

main:   
	sb $8, 0($3)
	sb $9, 4($4)
	sb $10, 0($5)
	syscall
