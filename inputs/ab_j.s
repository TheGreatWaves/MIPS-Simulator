        # Basic branch test
	.text

main:
        addiu $v0, $zero, 0xa # 00
        j exit                # 04
inf:
        j inf                 # 08
exit:
        syscall               # 0c




