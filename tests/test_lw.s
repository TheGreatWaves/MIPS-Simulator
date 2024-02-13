# THIS TEST FILE ASSUMES THAT THE DATA IS SET.

.text

main:   
	lw $8, 0($3)
	lw $9, 0($4)
	lw $10, 0($5)
	lw $11, 1($4)
	lw $12, 2($6)
	syscall
