# J JAL BEQ BNE BLEZ BGTZ
# SLTI SLTIU ANDI ORI
# XORI LUI LB LH LW LBU
# LHU SB SH SW BLTZ BGEZ
# BLTZAL BGEZAL SLL SRL SRA SLLV
# SRLV SRAV JR JALR ADD ADDU
# NOR
# SLT SLTU MULT MFHI MFLO MTHI
# MTLO MULTU DIV DIVU SYSCALL

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
done:
        j exit
inf:
        j inf
exit:
        addiu $v0, $zero, 0xa
        syscall

        
        
                        
