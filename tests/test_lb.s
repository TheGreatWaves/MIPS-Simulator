
# This test assumes that all jump and branching instruction works.

.text

main:   
test_lb:
	lb $8, 0($3)
	lb $9, 0($4)
	lb $10, 1($4)
        syscall
