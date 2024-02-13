
# This test assumes that all jump and branching instruction works.

.text

main:   
	sh $8, 0($3) # write 0xabcd
	sh $9, 4($4) # write 255
	sh $10, 0($5) # write -1
        syscall
