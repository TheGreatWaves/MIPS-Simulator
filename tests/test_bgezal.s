.text

main:   
test_bgezal_take:
	bgezal $3, bgezal_take_1
        addi $15, $0, -1
        syscall
bgezal_take_1:
	addi $8, $ra, 0 # record
	bgezal $4, error
	addi $9, $ra, 0 # record
	bgezal $5, bgezal_take_2
        addi $15, $0, -1
        syscall
bgezal_take_2:
	addi $10, $ra, 0 # record
        syscall
error:
        addi $15, $0, -1
        syscall
