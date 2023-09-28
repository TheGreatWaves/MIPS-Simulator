# THIS TEST FILE ASSUMES THAT THE DATA IS SET.

.text

main:   
	lui $t1, 0x1000
	lui $t0, 0x1000
	addi $t0, $t0, 0xa
	sb $t0, 0($t1)
	lh $v0, 0($t1)
exit:
        syscall
