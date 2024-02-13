

# This test assumes that all jump and branching instruction works.

.text

main:   
        lbu $8, 0($3)
        lbu $9, 0($4)
        lbu $10, 1($4)
        syscall
