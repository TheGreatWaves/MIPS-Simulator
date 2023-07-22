
.data
    base: .word 100   # Base value
    offset: .half -50  # Offset value

.text
    main:
        la $t0, base     # Load the address of base into $t0
        lhu $t1, offset  # Load the offset value into $t1 (unsigned half-word)

        sb $t1, 0($t0)   # Store the least-significant byte of $t1 at the effective address ($t0)

        # Print the updated value
        li $v0, 1        # Set $v0 to 1 for printing an integer
        lw $a0, 0($t0)   # Load the value at the effective address into $a0
        syscall         # Print the value

        # End the program
        li $v0, 10       # Set $v0 to 10 for program exit
        syscall         # Exit the program