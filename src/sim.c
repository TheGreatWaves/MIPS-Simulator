#include "shell.h"
#include "uninspiring_macros.h"
#include "utils.h"
#include <stdio.h>
#include <stdbool.h>

/////////////////////////////////////
// NOTE(Appy): Labels for organization
#define PIPE_LINE_STAGE
#define PIPE_LINE_REGISTER

/////////////////////////////////////
// NOTE(Appy): Registers

#define R_V0 2
#define R_RA 31

/////////////////////////////////////
// NOTE(Appy): Control Signals

typedef struct {
  u32 ALUSrc: 1;
  u32 ALUOp:  3;
} ExecuteControlSignals;

typedef struct {
  u32 MemRead:  1;
  u32 MemWrite: 1;
  u32 PCSrc:    1;
} MemoryControlSignals;

typedef struct {
  u32 RegWrite: 1;
  u32 RegDst: 1;
  u32 MemToReg: 1;
} WriteBackControlSignals;

/////////////////////////////////////
// NOTE(Appy): Pipeline Registers

/* IF/ID */
PIPE_LINE_REGISTER struct {
  u32 instruction;
  u32 pc;
} pr_if_id;

/* ID/EX */
PIPE_LINE_REGISTER struct {
  /* Data */
  u32 pc;   // Program counter
  u32 imm;  // Sign extended immediate

  /* Register index */
  u8  rsi;  
  u8  rti;  
  u8  rdi;

  /* Register values */
  u32 rsv;   // REGS[$RS] 
  u32 rtv;   // REGS[$RT]

  /* Control Signals */
  ExecuteControlSignals   ecs;
  MemoryControlSignals    mcs;
  WriteBackControlSignals wbcs;
} pr_id_ex;

PIPE_LINE_REGISTER struct 
{
  u32                     pc;
  u32                     alu_res;
  u32                     mem_res;
  u8                      rd;     // EX_WriteRegister
  u32                     target;
  MemoryControlSignals    mcs;
  WriteBackControlSignals wbcs;
  bool                    branch;
} pr_ex_mem;

PIPE_LINE_REGISTER struct 
{
  u32                     memory_read;
  u32                     alu_res;
  u32                     mem_res;
  u8                      rd;
  WriteBackControlSignals wbcs;
} pr_mem_wb;

/////////////////////////////////////
// NOTE(Appy): Pipeline Stages

PIPE_LINE_STAGE void fetch()
{
  // Read the instruction and store it inside the IF/ID pipeline register.

  // For later decoding.
  pr_if_id.instruction = mem_read_32(CURRENT_STATE.PC);

  // For later computing branch targets.
  pr_if_id.pc = CURRENT_STATE.PC + 4; 

  CURRENT_STATE.PC += 4;
}

inline void enable_memory_w()
{
  // Enable write to memory.
  pr_id_ex.mcs.MemWrite = MemRead_yes;

  // Disable memory read
  pr_id_ex.mcs.MemRead = MemRead_no;
}

inline void enable_memory_r()
{
  // Enable read to memory.
  pr_id_ex.mcs.MemRead = MemRead_yes;

  // Disable memory write
  pr_id_ex.mcs.MemWrite = MemRead_no;
}

inline void disable_memory()
{
  pr_id_ex.mcs.MemRead = MemRead_no;
  pr_id_ex.mcs.MemWrite = MemWrite_no;
}

inline void set_alu_op(u8 op)
{
  pr_id_ex.ecs.ALUOp = op;
}

inline void handle_rtype(u8 op)
{
  switch(op)
  {
    break; case SYSCALL:
    {
      gprint("RTYPE: SYSCALL\n");
      if (CURRENT_STATE.REGS[R_V0] == 0x0A) {
        RUN_BIT = FALSE;
      }
    }
  }
  
}

PIPE_LINE_STAGE void decode()
{
  // Forward the PC
  pr_id_ex.pc = pr_if_id.pc;

  // Store register numbers
  pr_id_ex.rsi = GET(RS, pr_if_id.instruction);
  pr_id_ex.rti = GET(RT, pr_if_id.instruction);

  // This is never on the RHS. We never need to store $RD.
  pr_id_ex.rdi = GET(RD, pr_if_id.instruction);

  // Store register values
  pr_id_ex.rtv = CURRENT_STATE.REGS[pr_id_ex.rti];
  pr_id_ex.rsv = CURRENT_STATE.REGS[pr_id_ex.rsi];

  // Indiscriminately sign extend the 16 bits immediate and store it.
  pr_id_ex.imm = sign_extend_16(u32t(GET(IM, pr_if_id.instruction)));

  // Set control signals
  enum EOPCODES instr = cast(enum EOPCODES, GET(OP, pr_if_id.instruction));

  switch (instr)
  {
    break; case SPECIAL: // R Type Instructions
    {
      // No need to write to memory
      disable_memory();

      pr_id_ex.wbcs.MemToReg = 0; // Select from ALU result to be written back.
      pr_id_ex.wbcs.RegWrite = 1; // We need to write to the register.

      // Execute
      pr_id_ex.ecs.ALUSrc    = 1; // 2nd ALU operands comes from the Register File.
      pr_id_ex.wbcs.RegDst   = RegDst_rd; // Destination register is $rd.

      // Find out the ALUOp
      u8 code = GET(CD, pr_if_id.instruction);

      handle_rtype(code);
    }

    break; case ADDI:
    {
      gprint("Decoded: ADDI\n");

      pr_id_ex.wbcs.RegDst = RegDst_rt;
      pr_id_ex.wbcs.RegWrite = RegWrite_yes;
      pr_id_ex.ecs.ALUSrc = ALUSrc_immediate;

      set_alu_op(ALUOp_ADD);

      disable_memory();

      pr_id_ex.wbcs.MemToReg = MemToReg_ALU_result;
    }


    break; case LW: // Load word
    {
      gprint("Decoded: LW\n");

      // We need to read the data (load!)
      enable_memory_r();

      pr_id_ex.wbcs.RegDst = RegDst_rt;
      pr_id_ex.wbcs.RegWrite = RegWrite_yes;

      pr_id_ex.ecs.ALUSrc = ALUSrc_immediate;
      pr_id_ex.ecs.ALUOp = ALUOp_ADD;

      pr_id_ex.wbcs.MemToReg = MemToReg_memory_data;

    }
    break; case SW:
    {
      enable_memory_w();

      // No need to set RegDst.
      // No need to set MemToReg.

      // Do not write to register.
      pr_id_ex.wbcs.RegWrite = 0;

      // Selects the immediate as the 2nd operand.
      pr_id_ex.ecs.ALUSrc = 0;

      pr_id_ex.ecs.ALUOp = 0b010;

    }
    break; case BEQ:
    {
      // Do not write to register.
      pr_id_ex.wbcs.RegWrite = 0;

      // Selects the immediate as the 2nd operand.
      pr_id_ex.ecs.ALUSrc = 1;

      pr_id_ex.ecs.ALUOp = 0b110;

      // No memory operation
      disable_memory();
    }
  }
}

// Decode ALU opcode.
// I have no idea what is going on here.
inline void decode_alu_op()
{
  
}

inline void choose_register_destination()
{
  // RegDist:
  //  0: $rt
  //  1: $rd
  pr_ex_mem.rd = (pr_id_ex.wbcs.RegDst == 0) 
                  ? pr_id_ex.rti 
                  : pr_id_ex.rdi;
}

inline void calculate_branch_target()
{
  pr_ex_mem.target = pr_id_ex.pc + pr_id_ex.imm;
}

inline void forward_control_id_ex_to_ex_mem()
{
  pr_ex_mem.mcs = pr_id_ex.mcs;
  pr_ex_mem.wbcs = pr_id_ex.wbcs;
}

inline u32 operand_a()
{
  return pr_id_ex.rsv;
}

inline u32 operand_b()
{
  if (pr_id_ex.ecs.ALUSrc == ALUSrc_immediate)
  {
    return pr_id_ex.imm;
  }
  else
  {
    return pr_id_ex.rtv;
  }
}

inline void set_alu_result(u32 v)
{
  pr_ex_mem.alu_res = v;
}

inline void execute_alu()
{
  switch (pr_id_ex.ecs.ALUOp)
  {
    break; case ALUOp_BEQ:
           case ALUOp_ADD: 
    {
      gprint("Execute: ADD\n");
      set_alu_result(operand_a() + operand_b());
    }
  }
}

// Decide whether jump/branch should be taken
inline void work_out_branch()
{
  
}

PIPE_LINE_STAGE void execute()
{
  forward_control_id_ex_to_ex_mem();

  if (pr_ex_mem.wbcs.RegWrite == RegWrite_yes)
  {
    gprint("Execute: WriteBack = yes\n");
  }

  choose_register_destination();
  execute_alu();
}

/////////////////////////////////////
// NOTE(Appy): Handlers

HANDLER(REGIMM) 
{
  u8 op = GET_BLOCK(INSTRUCTION_REGISTER, 16, 5);
  switch (op) 
  {
    case SRAV:
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

PIPE_LINE_STAGE void memory()
{
  pr_mem_wb.wbcs = pr_ex_mem.wbcs;

  if (pr_ex_mem.mcs.MemRead) 
  {
    // The ALU computes the (base + offset)
    pr_ex_mem.mem_res = mem_read_32(pr_ex_mem.alu_res);
  }

  if (pr_ex_mem.mcs.MemWrite)
  {
    
  }

  // EX_WriteRegister -> MEM_WriteRegister
  pr_mem_wb.rd = pr_ex_mem.rd;

  // EX_ALUResult -> MEM_ALUResult
  pr_mem_wb.alu_res = pr_ex_mem.alu_res;

}


inline u32 write_back_data()
{
  if (pr_mem_wb.wbcs.MemToReg == MemToReg_memory_data)
  {
    // TODO: return the load data...
    return pr_mem_wb.mem_res;
  }
  else
  {
    return pr_mem_wb.alu_res;
  }
}




PIPE_LINE_STAGE void writeback()
{
  if (pr_mem_wb.wbcs.RegWrite == RegWrite_yes)
  {
    gprint("Writing back\n");
    gprint(pr_mem_wb.rd);
    gprint(" ");
    gprint(write_back_data());
    CURRENT_STATE.REGS[pr_mem_wb.rd] = write_back_data();
  }

  NEXT_STATE = CURRENT_STATE; 
}

void process_instruction() {
  /* Instruction jump tables */
  // static const void *jumpTable[] = {OPCODES(MK_LBL) MK_LBL(NEXT_STATE)};

  fetch();

  decode();

  execute();

  memory();

  writeback();
  

//   uint32_t INSTRUCTION_REGISTER = pr_if_id.instruction;

//   /* Retrieve the opcode of the instruction. */
//   uint8_t instr = GET(OP, INSTRUCTION_REGISTER);

// /* Helpers */
// #define END_LABEL LBL(NEXT_STATE) :
// #define RS (CURRENT_STATE.REGS[GET(RS, INSTRUCTION_REGISTER)])
// #define RT (NEXT_STATE.REGS[GET(RT, INSTRUCTION_REGISTER)])
// #define IMM (GET(IM, INSTRUCTION_REGISTER))

//   /* Dispatch the instruction. */
//   DISPATCH(instr) {
//     /* First six bits are 0s */
//     LBL(SPECIAL) : {
//       CALL_HANDLER(SPECIAL);
//       NEXT;
//     }
//     LBL(REGIMM) : {
//       CALL_HANDLER(REGIMM);
//       NEXT;
//     }

//     /* Basic cases, none of these messes control flow */
//     LBL(ADDI) : LBL(ADDIU) : {
//       uint32_t result = sign_extend_16(u32t(IMM));
//       RT = RS + result;
//       NEXT;
//     }
//     LBL(XORI) : {
//       RT = RS ^ u32t(IMM);
//       NEXT;
//     }
//     LBL(ANDI) : {
//       RT = RS & u32t(IMM);
//       NEXT;
//     }
//     LBL(ORI) : {
//       RT = RS | u32t(IMM);
//       NEXT;
//     }
//     LBL(LUI) : {
//       RT = (IMM << 16);
//       NEXT;
//     }
//     LBL(J) : {
//       u32 jp = GET_BLOCK(INSTRUCTION_REGISTER, 0, 26);
//       CURRENT_STATE.PC = ((jp << 2) - 4);
//       NEXT;
//     }

//     /* Default case. */
//     LBL(PADDING) : { NEXT; }
//     LBL(SB) : {
//       uint32_t offset = sign_extend_16(IMM);
//       uint32_t address = offset + RS;
//       uint32_t r_rt = CURRENT_STATE.REGS[GET(RT, INSTRUCTION_REGISTER)];
//       uint32_t last_byte = (GET_BLOCK(r_rt, 0, 8));
//       mem_write_32(address, last_byte);
//       NEXT;
//     }
//     LBL(SH) : {
//       uint32_t offset = sign_extend_16(IMM);
//       uint32_t address = offset + RS;
//       uint32_t r_rt = CURRENT_STATE.REGS[GET(RT, INSTRUCTION_REGISTER)];
//       uint32_t last_byte = (GET_BLOCK(r_rt, 0, 16));
//       mem_write_32(address, last_byte);
//       NEXT;
//     }
//     LBL(SW) : {
//       u32 vAddr = RS + sign_extend_16(IMM);
//       mem_write_32(vAddr, RT);
//       NEXT;
//     }
//     LBL(SLTI) : {
//       RT = (cast(s32, RS) < cast(s32, sign_extend_16(IMM))) ? 1 : 0;
//       NEXT;
//     }
//     LBL(SLTIU) : {
//       u32 result = RS - sign_extend_16(IMM);
//       RT = (RS < result) ? 1 : 0;
//       NEXT;
//     }
//     LBL(LB) : {
//       u32 vAddr = RS + sign_extend_16(IMM);
//       u32 content = mem_read_32(vAddr);
//       u32 byte = GET_BLOCK(content, 0, 8);

//       u32 byte_extended = sign_extend_8(byte);
//       RT = byte_extended;
//       NEXT;
//     }
//     LBL(LBU) : {
//       u32 vAddr = RS + sign_extend_16(IMM);
//       u32 content = mem_read_32(vAddr);
//       u32 byte = GET_BLOCK(content, 0, 8);
//       RT = byte;
//       NEXT;
//     }
//     LBL(LH) : {
//       u32 vAddr = RS + sign_extend_16(IMM);
//       u32 content = mem_read_32(vAddr);
//       u32 byte = GET_BLOCK(content, 0, 16);

//       u32 byte_extended = sign_extend_16(byte);
//       RT = byte_extended;
//       NEXT;
//     }
//     LBL(LHU) : {
//       u32 vAddr = RS + sign_extend_16(IMM);
//       u32 content = mem_read_32(vAddr);
//       u32 byte = GET_BLOCK(content, 0, 16);
//       RT = byte;
//       NEXT;
//     }
//     LBL(LW) : {
//       u32 vAddr = RS + sign_extend_16(IMM);
//       RT = mem_read_32(vAddr);
//       NEXT;
//     }
//     LBL(JAL) : {
//       u32 temp = (GET_BLOCK(INSTRUCTION_REGISTER, 0, 25) << 2);
//       NEXT_STATE.REGS[R_RA] = CURRENT_STATE.PC + 4;
//       CURRENT_STATE.PC = temp - 4;
//       NEXT;
//     }
//     LBL(BEQ) : {
//       u32 rs = RS;
//       u32 rt = RT;
//       u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);
//       if (rs == rt) {
//         CURRENT_STATE.PC = addr - 4;
//       }
//       NEXT;
//     }
//     LBL(BNE) : {
//       u32 rs = RS;
//       u32 rt = RT;
//       u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

//       if (rs != rt) {
//         CURRENT_STATE.PC = addr - 4;
//       }

//       NEXT;
//     }
//     LBL(BLEZ) : {
//       u32 rs = RS;
//       u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

//       if (rs == 0 || ((rs >> 31) == 1)) {
//         CURRENT_STATE.PC = addr - 4;
//       }

//       NEXT;
//     }
//     LBL(BGTZ) : {
//       u32 rs = RS;
//       u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

//       if (rs != 0 && ((rs >> 31) == 0)) {
//         CURRENT_STATE.PC = addr - 4;
//       }
//       NEXT;
//     }
//   }

//   /* This is place in this way in order to allow disabling of threaded code */
//   END_LABEL {
//     /* Increment the program counter */
//     NEXT_STATE.PC = CURRENT_STATE.PC + 4;
//   }

// /* Undefine guards */
// #undef END_LABEL

// #ifdef RS
// #undef RS
// #endif

// #ifdef RT
// #undef RT
// #endif

// #ifdef IMM
// #undef IMM
// #endif
}
