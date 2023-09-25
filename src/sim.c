#include "shell.h"
#include "uninspiring_macros.h"
#include <stdio.h>

/////////////////////////////////////
// NOTE(Appy): Registers

#define R_V0 2
#define R_RA 31

/////////////////////////////////////
// NOTE(Appy): Handlers

HANDLER(REGIMM) {
  u8 op = GET_BLOCK(INSTRUCTION_REGISTER, 16, 5);
  switch (op) {
  case BLTZ: {
    u32 addr = CURRENT_STATE.PC + (sign_extend_16(GET(IM, INSTRUCTION_REGISTER)) << 2);
    int branch = (CURRENT_STATE.REGS[GET(RS, INSTRUCTION_REGISTER)] >> 31);
    if (branch == 1) {
      CURRENT_STATE.PC = addr - 4;
    }
    break;
  }
  case BLTZAL: {
    u32 addr = CURRENT_STATE.PC + (sign_extend_16(GET(IM, INSTRUCTION_REGISTER)) << 2);
    int branch = (CURRENT_STATE.REGS[GET(RS, INSTRUCTION_REGISTER)] >> 31);
    NEXT_STATE.REGS[R_RA] = CURRENT_STATE.PC + 4;
    if (branch == 1) {
      CURRENT_STATE.PC = addr - 4;
    }
    break;
  }
  case BGEZ: {
    u32 addr = CURRENT_STATE.PC + (sign_extend_16(GET(IM, INSTRUCTION_REGISTER)) << 2);
    int branch = (CURRENT_STATE.REGS[GET(RS, INSTRUCTION_REGISTER)] >> 31);
    if (branch == 0) {
      CURRENT_STATE.PC = addr - 4;
    }
    break;
  }
  case BGEZAL: {
    u32 addr = CURRENT_STATE.PC + (sign_extend_16(GET(IM, INSTRUCTION_REGISTER)) << 2);
    int branch = (CURRENT_STATE.REGS[GET(RS, INSTRUCTION_REGISTER)] >> 31);
    NEXT_STATE.REGS[R_RA] = CURRENT_STATE.PC + 4;
    if (branch == 0) {
      CURRENT_STATE.PC = addr - 4;
    }
    break;
  }
  default:
    break;
  }
}

HANDLER(SPECIAL) {
  uint8_t code = GET(CD, INSTRUCTION_REGISTER);

/* Undefine register helpers from process instruction */
#undef RS
#undef RT
#undef IMM

/* Register helpers */
#define RD (NEXT_STATE.REGS[GET(RD, INSTRUCTION_REGISTER)])
#define RS (CURRENT_STATE.REGS[GET(RS, INSTRUCTION_REGISTER)])
#define RT (CURRENT_STATE.REGS[GET(RT, INSTRUCTION_REGISTER)])
#define SA (GET(SA, INSTRUCTION_REGISTER))

  switch (code) {
  case SYSCALL: // System call
  {

    if (CURRENT_STATE.REGS[R_V0] == 0x0A) {
      RUN_BIT = FALSE;
    }
    /* else do nothing... increment as usual */
    break;
  }
  case ADDU:
  case ADD: {
    RD = RS + RT;
    break;
  } // Addition
  case SUB: {
    RD = RS - RT;
    break;
  } // Subtraction
  case OR: {
    RD = RS | RT;
    break;
  } // Bit Or
  case AND: {
    RD = RS & RT;
    break;
  } // Bit And
  case SUBU: {
    RD = RS - RT;
    break;
  } // Subtraction Unsigned
  case XOR: {
    RD = RS ^ RT;
    break;
  } // Exclusive Or
  case NOR: {
    RD = NOR_OP(RS, RT);
    break;
  } // Nor
  case JR: {
    CURRENT_STATE.PC = RS - 4;
    break;
  }
  case JALR: {
    RD = CURRENT_STATE.PC + 4;
    CURRENT_STATE.PC = RS - 4;
    break;
  }
  case MTLO: {
    NEXT_STATE.LO = RS;
    break;
  }
  case MTHI: {
    NEXT_STATE.HI = RS;
    break;
  }
  case MFHI: {
    RD = CURRENT_STATE.HI;
    break;
  }
  case MFLO: {
    RD = CURRENT_STATE.LO;
    break;
  }
  case MULT: {
    s32 rs     = RS;
    s32 rt     = RT;
    s64 result = rs * rt;
    NEXT_STATE.HI = cast(u32, (result >> 32));
    NEXT_STATE.LO = cast(u32, result);
    break;
  }
  case MULTU: {
    u64 result = cast(u64, RS) * cast(u64, RT);
    NEXT_STATE.HI = cast(u32, (result >> 32));
    NEXT_STATE.LO = cast(u32, result);
    break;
  }
  case SLLV:
  {
    u8 shift_amount = (RS & MASK(5));
    RD = (RT << shift_amount);
    break;
  }
  case SLT:
  {
    s32 rt = RT;
    s32 rs = RS;
    RD = (rs<rt) ? 1 : 0;
    break;
  }
  case SLTU:
  {
    u32 rt = RT;
    u32 rs = RS;
    RD = (rs<rt) ? 1 : 0;
    break;
  }
  case SRAV:
  {
    u8 shift_amount = (RS & MASK(5));
    u32 rt = RT;
    u8 sign_bit = ((rt >> 31) & 1);
    u32 res = (rt >> shift_amount);
    if (sign_bit==1) res |= (MASK(shift_amount) << (32-shift_amount));
    RD = res;
    break;
  }
  case SRLV:
  {
    u8 shift_amount = (RS & MASK(5));
    RD = (RT >> shift_amount);
    break;
  }
  case DIV: {
    s32 numerator   = RS;
    s32 denominator = RT;
    if (denominator != 0) {
      NEXT_STATE.LO = u32t(numerator / denominator);
      NEXT_STATE.HI = u32t(numerator % denominator);
    }
    break;
  }
  case DIVU: {
    u32 denominator = RT;
    u32 numerator = RS;
    if (denominator != 0) {
      NEXT_STATE.LO = u32t(numerator / denominator);
      NEXT_STATE.HI = u32t(numerator % denominator);
    }
    break;
  }
  case SLL: {
    RD = RT << SA;
    break;
  } // Shift Left
  case SRL: {
    RD = RT >> SA;
    break;
  }         // Shirt Right
  case SRA: // Shift Right Addition
  {
    uint8_t sa = SA;
    uint32_t operand = RT;

    int need_extend = (operand >> 31) & 1;
    uint32_t result = (operand >> sa);

    if (need_extend) {
      /* Sign extension */
      result = result | MASK1(sa, INSTR_SIZE - sa);
    }

    RD = result;
    break;
  }
  }

#undef RD
#undef RS
#undef RT
#undef SA
}

void process_instruction() {
  /* Instruction jump tables */
  static const void *jumpTable[] = {OPCODES(MK_LBL) MK_LBL(NEXT_STATE)};

  /* Fetch */
  uint32_t INSTRUCTION_REGISTER = mem_read_32(CURRENT_STATE.PC);

  /* Retrieve the opcode of the instruction. */
  uint8_t instr = GET(OP, INSTRUCTION_REGISTER);

/* Helpers */
#define END_LABEL LBL(NEXT_STATE) :
#define RS (CURRENT_STATE.REGS[GET(RS, INSTRUCTION_REGISTER)])
#define RT (NEXT_STATE.REGS[GET(RT, INSTRUCTION_REGISTER)])
#define IMM (GET(IM, INSTRUCTION_REGISTER))

  /* Dispatch the instruction. */
  DISPATCH(instr) {
    /* First six bits are 0s */
    LBL(SPECIAL) : {
      CALL_HANDLER(SPECIAL);
      NEXT;
    }
    LBL(REGIMM) : {
      CALL_HANDLER(REGIMM);
      NEXT;
    }

    /* Basic cases, none of these messes control flow */
    LBL(ADDI) : LBL(ADDIU) : {
      uint32_t result = sign_extend_16(u32t(IMM));
      RT = RS + result;
      NEXT;
    }
    LBL(XORI) : {
      RT = RS ^ u32t(IMM);
      NEXT;
    }
    LBL(ANDI) : {
      RT = RS & u32t(IMM);
      NEXT;
    }
    LBL(ORI) : {
      RT = RS | u32t(IMM);
      NEXT;
    }
    LBL(LUI) : {
      RT = (IMM << 16);
      NEXT;
    }
    LBL(J) : {
      u32 jp = GET_BLOCK(INSTRUCTION_REGISTER, 0, 26);
      CURRENT_STATE.PC = ((jp << 2) - 4);
      NEXT;
    }

    /* Default case. */
    LBL(PADDING) : { NEXT; }
    LBL(SB) : {
      uint32_t offset = sign_extend_16(IMM);
      uint32_t address = offset + RS;
      uint32_t r_rt = CURRENT_STATE.REGS[GET(RT, INSTRUCTION_REGISTER)];
      uint32_t last_byte = (GET_BLOCK(r_rt, 0, 8));
      mem_write_32(address, last_byte);
      NEXT;
    }
    LBL(SH) : {
      uint32_t offset = sign_extend_16(IMM);
      uint32_t address = offset + RS;
      uint32_t r_rt = CURRENT_STATE.REGS[GET(RT, INSTRUCTION_REGISTER)];
      uint32_t last_byte = (GET_BLOCK(r_rt, 0, 16));
      mem_write_32(address, last_byte);
      NEXT;
    }
    LBL(SW) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      mem_write_32(vAddr, RT);
      NEXT;
    }
    LBL(SLTI) : {
      RT = (cast(s32, RS) < cast(s32, sign_extend_16(IMM))) ? 1 : 0;
      NEXT;
    }
    LBL(SLTIU) : {
      u32 result = RS - sign_extend_16(IMM);
      RT = (RS < result) ? 1 : 0;
      NEXT;
    }
    LBL(LB) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      u32 content = mem_read_32(vAddr);
      u32 byte = GET_BLOCK(content, 0, 8);

      u32 byte_extended = sign_extend_8(byte);
      RT = byte_extended;
      NEXT;
    }
    LBL(LBU) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      u32 content = mem_read_32(vAddr);
      u32 byte = GET_BLOCK(content, 0, 8);
      RT = byte;
      NEXT;
    }
    LBL(LH) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      u32 content = mem_read_32(vAddr);
      u32 byte = GET_BLOCK(content, 0, 16);

      u32 byte_extended = sign_extend_16(byte);
      RT = byte_extended;
      NEXT;
    }
    LBL(LHU) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      u32 content = mem_read_32(vAddr);
      u32 byte = GET_BLOCK(content, 0, 16);
      RT = byte;
      NEXT;
    }
    LBL(LW) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      RT = mem_read_32(vAddr);
      NEXT;
    }
    LBL(JAL) : {
      u32 temp = (GET_BLOCK(INSTRUCTION_REGISTER, 0, 25) << 2);
      NEXT_STATE.REGS[R_RA] = CURRENT_STATE.PC + 4;
      CURRENT_STATE.PC = temp - 4;
      NEXT;
    }
    LBL(BEQ) : {
      u32 rs = RS;
      u32 rt = RT;
      u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);
      if (rs == rt) {
        CURRENT_STATE.PC = addr - 4;
      }
      NEXT;
    }
    LBL(BNE) : {
      u32 rs = RS;
      u32 rt = RT;
      u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

      if (rs != rt) {
        CURRENT_STATE.PC = addr - 4;
      }

      NEXT;
    }
    LBL(BLEZ) : {
      u32 rs = RS;
      u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

      if (rs == 0 || ((rs >> 31) == 1)) {
        CURRENT_STATE.PC = addr - 4;
      }

      NEXT;
    }
    LBL(BGTZ) : {
      u32 rs = RS;
      u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

      if (rs != 0 && ((rs >> 31) == 0)) {
        CURRENT_STATE.PC = addr - 4;
      }
      NEXT;
    }
  }

  /* This is place in this way in order to allow disabling of threaded code */
  END_LABEL {
    /* Increment the program counter */
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
  }

/* Undefine guards */
#undef END_LABEL

#ifdef RS
#undef RS
#endif

#ifdef RT
#undef RT
#endif

#ifdef IMM
#undef IMM
#endif
}
