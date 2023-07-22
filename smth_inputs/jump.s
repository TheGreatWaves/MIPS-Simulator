
        # Basic arithmetic instructions
        # This is a hodgepodge of arithmetic instructions to test
        # your basic functionality.
        # No overflow exceptions should occur
	.text
main:   
        addiu   $3, $zero, 0x00400010
        jr      $3
        addiu   $4, $2, $2 
        addiu   $5, $zero, 100 
        addiu   $6, $zero, 200 
        addiu   $2, $zero, 0xa
        syscall
        
        
                        

