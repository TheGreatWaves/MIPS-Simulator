# This test assumes that all jump and branching instruction works.

.text

# $3 == 0
# $4 == 256
# $5 == -1
# $6 == -256

main:   
	# $3 is 0, take.
	bgez $3, bgez_take_1
	addi $15, $0, -1
	syscall
bgez_take_1:
	# $4 is 256, take.
	bgez $4, bgez_take_2
	addi $15, $0, -1
	syscall
bgez_take_2:
	bgez $5, inf
	bgez $6, inf
	syscall
inf:
	addi $15, $0, -1
        syscall
