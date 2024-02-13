.text

main:   
        addi $t0, $zero, 0xbef
        mthi $t0
exit:
        addi $v0, $zero, 0xa
        syscall
