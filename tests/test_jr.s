


# This test assumes that all jump and branching instruction works.

.text

main:   
        lui $t3, 0x0040        # 0x00
        ori $t3, $t3, 0x18     # 0x04
        addiu $t0, $zero, 255  # 0x08
        jr exit                # 0x0c 
        addiu $t0, $zero, 5    # 0x10
        addiu $t1, $zero, 10   # 0x14
exit:
        addiu $v0, $zero, 0xa  # 0x18
        syscall
