
        # Basic arithmetic instructions
        # This is a hodgepodge of arithmetic instructions to test
        # your basic functionality.
        # No overflow exceptions should occur
	.text
main:   
        addiu   $2, $zero, 512
        addu    $3, $2, $2
	div 	$2, $3
        addiu   $2, $zero, 0xa
        syscall
        
        
                        
