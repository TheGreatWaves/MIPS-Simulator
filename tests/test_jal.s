# This test assumes that all jump and branching instruction works.

.text

main:   
        lui $t1, 0x0040
        ori $t1, $t1, 0xc
        jal exit                                 # 0x00
        addi $t0, $0, 5
exit:
        beq $t1, $31, nice
oops:
        j oops
nice:
        addiu $v0, $zero, 0xa                  # 0x14
        syscall
