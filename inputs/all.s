# BLEZ BGTZ
# SLTI SLTIU ANDI ORI
# XORI LUI LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SLL SRL SRA SLLV
# SRLV SRAV JALR 
# SLT SLTU    
# DIV DIVU 

# This test assumes that all jump and branching instruction works.

.text

main:   
        j start_test
reset:
        addi $t0, $zero, 0
        addi $t1, $zero, 0
        addi $t2, $zero, 0
        addi $t3, $zero, 0
        addi $t4, $zero, 0
        mthi $zero
        mtlo $zero
        jr $ra
start_test:
test_addi_pos:
        # test ADDI positive
        addi $t0, $t0, 0x1a
        ori $t1, $t1, 0x1a
        bne $t0, $t1, inf
        jal reset
test_addi_neg:
        # test ADDI negative
        addi $t0, $t0, -1
        lui $t1, 0xFFFF
        ori $t1, $t1, 0xFFFF
        bne $t0, $t1, inf
        jal reset
test_add_pos:
        # test add positive
        ori $t1, $t1, 0x1a
        add $t0, $t0, $t1
        bne $t0, $t1, inf
        jal reset
test_add_neg:
        # test add negative
        addi $t2, $t2, -1
        add $t0, $t0, $t2
        lui $t1, 0xFFFF
        ori $t1, $t1, 0xFFFF
        bne $t0, $t1, inf
        jal reset
test_addu_pos:
        # test addu positive
        ori $t1, $t1, 0x1a
        addu $t0, $t0, $t1
        bne $t0, $t1, inf
        jal reset
test_addu_neg:
        # test addu negative
        addi $t2, $t2, -1
        addu $t0, $t0, $t2
        lui $t1, 0xFFFF
        ori $t1, $t1, 0xFFFF
        bne $t0, $t1, inf
        jal reset
test_addiu_pos:
        # test ADDIU positive
        addiu $t0, $t0, 0x1a
        ori $t1, $t1, 0x1a
        bne $t0, $t1, inf
        jal reset
test_addiu_neg:
        # test ADDIU negative
        addiu $t0, $t0, -1
        lui $t1, 0xFFFF
        ori $t1, $t1, 0xFFFF
        bne $t0, $t1, inf
        jal reset
test_sub:
        # test sub (pos result)
        ori $t0, $t0, 0xFF
        ori $t1, $t1, 0x4
        sub $t3, $t0, $t1
        ori $t4, $t4, 0xFB
        bne $t3, $t4, inf
        jal reset
test_sub_neg:
        # test sub (neg result)
        ori $t0, $t0, 0x0
        ori $t1, $t1, 0x4
        sub $t3, $t0, $t1
        lui $t4, 0xFFFF
        ori $t4, $t4, 0xFFFC
        bne $t3, $t4, inf
        jal reset
test_sub_neg_1:
        # test sub (neg result)
        ori $t0, $t0, 0x4
        ori $t1, $t1, 0x0
        sub $t3, $t0, $t1
        addi $t4, $zero, 4
        bne $t3, $t4, inf
        jal reset
 test_subu:
         # test subu (pos result)
         ori $t0, $t0, 0xFF
         ori $t1, $t1, 0x4
         subu $t3, $t0, $t1
         ori $t4, $t4, 0xFB
         bne $t3, $t4, inf
         jal reset
test_subu_neg:
        # test subu (neg result)
        ori $t0, $t0, 0x0
        ori $t1, $t1, 0x4
        subu $t3, $t0, $t1
        lui $t4, 0xFFFF
        ori $t4, $t4, 0xFFFC
        bne $t3, $t4, inf
        jal reset
test_subu_neg_1:
        # test subu (neg result)
        ori $t0, $t0, 0x4
        ori $t1, $t1, 0x0
        subu $t3, $t0, $t1
        addi $t4, $zero, 4
        bne $t3, $t4, inf
        jal reset
test_and_00:
        addi $t0, $zero, 0
        addi $t1, $zero, 0
        and $t2, $t0, $t1
        bne $t2, $zero, inf
        jal reset
test_and_01:
        addi $t0, $zero, 0
        addi $t1, $zero, 1
        and $t2, $t0, $t1
        bne $t2, $zero, inf
        jal reset
test_and_11:
        addi $t0, $zero, 1
        addi $t1, $zero, 1
        and $t2, $t0, $t1
        beq $t2, $zero, inf
        jal reset
test_and_10:
        addi $t0, $zero, 1
        addi $t1, $zero, 0
        and $t2, $t0, $t1
        bne $t2, $zero, inf
        jal reset
test_or_00:
        addi $t0, $zero, 0
        addi $t1, $zero, 0
        or $t2, $t0, $t1
        bne $t2, $zero, inf
        jal reset
test_or_01:
        addi $t0, $zero, 0
        addi $t1, $zero, 1
        or $t2, $t0, $t1
        beq $t2, $zero, inf
        jal reset
test_or_11:
        addi $t0, $zero, 1
        addi $t1, $zero, 1
        or $t2, $t0, $t1
        beq $t2, $zero, inf
        jal reset
test_or_10:
        addi $t0, $zero, 1
        addi $t1, $zero, 0
        or $t2, $t0, $t1
        beq $t2, $zero, inf
        jal reset
test_xor_00:
        addi $t0, $zero, 0
        addi $t1, $zero, 0
        xor $t2, $t0, $t1
        bne $t2, $zero, inf
        jal reset
test_xor_01:
        addi $t0, $zero, 0
        addi $t1, $zero, 1
        xor $t2, $t0, $t1
        beq $t2, $zero, inf
        jal reset
test_xor_11:
        addi $t0, $zero, 1
        addi $t1, $zero, 1
        xor $t2, $t0, $t1
        bne $t2, $zero, inf
        jal reset
test_xor_10:
        addi $t0, $zero, 1
        addi $t1, $zero, 0
        xor $t2, $t0, $t1
        beq $t2, $zero, inf
        jal reset
test_nor_00:
        addi $t0, $zero, 0
        addi $t1, $zero, 0
        addi $t3, $zero, -1
        nor $t2, $t0, $t1
        bne $t2, $t3, inf
        jal reset
test_nor_01:
        addi $t0, $zero, 0
        addi $t1, $zero, 1
        addi $t3, $zero, -2
        nor $t2, $t0, $t1
        bne $t2, $t3, inf
        jal reset
test_nor_11:
        addi $t0, $zero, 1
        addi $t1, $zero, 1
        addi $t3, $zero, -2
        nor $t2, $t0, $t1
        bne $t2, $t3, inf
        jal reset
test_nor_10:
        addi $t0, $zero, 1
        addi $t1, $zero, 0
        addi $t3, $zero, -2
        nor $t2, $t0, $t1
        bne $t2, $t3, inf
        jal reset
test_mthi_mfhi:
        addi $t0, $zero, 0xbeef
        mthi $t0
        mfhi $t2
        bne $t0, $t2, inf
        mthi $zero
        jal reset
test_mtlo_mflo:
        addi $t0, $zero, 0xdead
        mtlo $t0
        mflo $t2
        bne $t0, $t2, inf
        mtlo $zero
        jal reset
test_mult_pos:
        addi $t0, $t0, 333        
        addi $t1, $t1, 111
        mult $t1, $t0
        mfhi $t2
        mflo $t3
        ori $t4, $t4, 0x9063
        bne $t2, $zero, inf
        bne $t3, $t4, inf
        jal reset
test_mult_neg:
        addi $t0, $t0, 333        
        addi $t1, $t1, -111
        mult $t1, $t0
        mfhi $t2
        mflo $t3
        ori $t1, $zero, -1
        ori $t4, $t4, -36963
        bne $t2, $t1, inf
        bne $t3, $t4, inf
        jal reset
test_multu_pos:
        addi $t0, $t0, 333        
        addi $t1, $t1, -111
        multu $t1, $t0
        mfhi $t2
        mflo $t3
        ori $t1, $zero, 0x14c # testing hi
        bne $t2, $t1, inf
        lui $t4, 0xffff       # testing lo
        ori $t4, $t4, 0x6f9d
        bne $t3, $t4, inf
        jal reset
done:
        j exit
inf:
        j inf
exit:
        addiu $v0, $zero, 0xa
        syscall

        
        
                        
