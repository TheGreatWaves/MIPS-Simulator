#include "shell.h"
#include "uninspiring_macros.h"
#include "utils.h"
#include <stdio.h>
#include <stdbool.h>

MAKE_SIGN_EXTEND(16)
MAKE_SIGN_EXTEND(8)

char pipeline_history[HISTORY_BUFFER_SIZE];
u32 fetch_count     = 0;
u32 decode_count    = 0;
u32 execute_count   = 0;
u32 memory_count    = 0;
u32 writeback_count = 0;

u32 cycle_number = 0;

/////////////////////////////////////
// NOTE(Appy): Pipeline stalls

Stall status = {
  .fetch     = STATUS_READY, // We kickstart fetching right away.
  .decode    = STATUS_STALL,
  .execute   = STATUS_STALL,
  .memory    = STATUS_STALL,
  .writeback = STATUS_STALL,  
};

// All of the registers starts off being ready.
bool REG_STATUS[MIPS_REGS+2] = 
{
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
  REG_READY,
}; 

/////////////////////////////////////
// NOTE(Appy): Pipeline Registers

PR_IF_ID pr_if_id;
PR_ID_EX pr_id_ex;
PR_EX_MEM pr_ex_mem;
PR_MEM_WB pr_mem_wb;

/////////////////////////////////////
// NOTE(Appy): Pipeline Stages

PIPE_LINE_STAGE void fetch()
{
  // Read the instruction and store it inside the IF/ID pipeline register.

  // For later decoding.
  pr_if_id.instruction = mem_read_32(CURRENT_STATE.PC);


  // For later computing branch targets.
  pr_if_id.pc = CURRENT_STATE.PC; 

  dprint("Fetching...address: 0x%x, got: 0x%x\n", pr_if_id.pc, pr_if_id.instruction);

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

void handle_rtype(u8 op)
{
  // Set the ALUOp.
  dprint("Hanlding RTYPE: %u\n", op);

  switch(op)
  {
    break; case SYSCALL:
    {
      if (CURRENT_STATE.REGS[R_V0] == 0x0A) {
        dprint("V0 == 0xA, terminating...%s\n", "");

        // We can stop fetching.
        status.fetch = STATUS_STALL;
        RUN_BIT = FALSE;
      }
    }
    break; case ADD:
           case ADDU:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_ADD;
    }
    break; case SUB:
           case SUBU:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_SUB;
    }
    break; case SLL:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_SLL;
    }
    break; case SLLV:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_SLLV;
    }
    break; case SRL:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_SRL;
    }
    break; case SRLV:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_SRLV;
    }
    break; case SRA:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_SRA;
    }
    break; case SRAV:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_SRAV;
    }
    break; case MTLO:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_ADD;
    }
    break; case MTHI:
    {
      pr_id_ex.ecs.ALUOp = ALUOp_ADD;
    }
    break; case JR:
    {
      dprint("Handling %s\n", "JR");
      pr_id_ex.ja = pr_id_ex.rsv;
      pr_id_ex.wbcs.PCSrc = PCSrc_jump; 

      // Stall fetch and decode.
      status.fetch = STATUS_STALL;
      status.decode = STATUS_STALL;
    }

    break; case JALR:
    {
      dprint("Handling %s\n", "JALR");
      pr_id_ex.ja = pr_id_ex.rsv;
      pr_id_ex.wbcs.PCSrc = PCSrc_jump; 

      // I don't know why but I'm going to forward it to add
      pr_id_ex.wbcs.RegDst = RegDst_rd;
      pr_id_ex.wbcs.RegWrite = RegWrite_yes;

      pr_id_ex.ecs.ALUOp = ALUOp_ADD;
      pr_id_ex.rsv = 4;
      pr_id_ex.rtv = pr_id_ex.pc;

      pr_id_ex.rdi = 31;

      // Stall fetch and decode.
      status.fetch = STATUS_STALL;
      status.decode = STATUS_STALL;
    }
  }
}

// Returns TRUE if there is a dependency issue, false otherwise.
inline bool has_dependency()
{
  enum EOPCODES instr = cast(enum EOPCODES, GET(OP, pr_if_id.instruction));

  switch (instr)
  {
    break; case SPECIAL:
    {
      u8 code = GET(CD, pr_if_id.instruction);

      switch (code)
      {
        break; case SYSCALL:
        {
          dprint("Data harzard detected for syscall, stalling %s\n", "decode");
          return REG_STATUS[R_V0] == REG_NOT_READY;
        }
        break; case MFHI:
        {
          return REG_STATUS[R_HI] == REG_NOT_READY;
        }
        break; case MFLO:
        {
          return REG_STATUS[R_LO] == REG_NOT_READY;
        }
      }
    }
  }
  
  // One of the registers we're reading from is not ready.
  u8 rti = GET(RT, pr_if_id.instruction);
  u8 rsi = GET(RS, pr_if_id.instruction);

  if (REG_STATUS[rti] == REG_NOT_READY)
  {
    dprint("Dependency for RT register: %u\n", rsi);
  }

  if (REG_STATUS[rsi] == REG_NOT_READY)
  {
    dprint("Dependency for RS register: %u\n", rti);
  }

  return (REG_STATUS[rti] == REG_NOT_READY) || (REG_STATUS[rsi] == REG_NOT_READY);
}


inline void set_register_dependency()
{
  // If we are going to write back then we must set 
  // the status of the register to not ready.
  if (pr_id_ex.wbcs.RegWrite == RegWrite_yes)
  {
    // Find out which register we are writing to.
    switch (pr_id_ex.wbcs.RegDst)
    {
      break; case RegDst_hi: REG_STATUS[R_HI] = REG_NOT_READY;
      break; case RegDst_lo: REG_STATUS[R_LO] = REG_NOT_READY; 
      break; case RegDst_rt: REG_STATUS[pr_id_ex.rti] = REG_NOT_READY; dprint("Register %u set to not ready\n", pr_id_ex.rti);
      break; case RegDst_rd: REG_STATUS[pr_id_ex.rdi] = REG_NOT_READY; dprint("Register %u set to not ready\n", pr_id_ex.rdi);
    }
  }
}

inline void link_next_pc()
{
  CURRENT_STATE.REGS[31] = pr_id_ex.pc + 4;
  REG_STATUS[31] = REG_NOT_READY;
}

// Read registers and sign extend the immediate and store them.
inline void retrieve_values()
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

  dprint("RT loaded: %d from register %d\n", CURRENT_STATE.REGS[pr_id_ex.rti], pr_id_ex.rti);
  dprint("RS loaded: %d from register %d\n", CURRENT_STATE.REGS[pr_id_ex.rsi], pr_id_ex.rsi);

  // Indiscriminately sign extend the 16 bits immediate and store it.
  pr_id_ex.imm = sign_extend_16(u32t(GET(IM, pr_if_id.instruction)));

  u32 jump_offset = GET_BLOCK(pr_id_ex.imm, 0, 26);
  pr_id_ex.ja = (jump_offset << 2);
}


/////////////////////////////////////
// NOTE(Appy): Templates

inline void template_load_instruction()
{
  pr_id_ex.ecs.ALUSrc = ALUSrc_immediate;
  pr_id_ex.ecs.ALUOp = ALUOp_ADD;

  // We need to read the data (load!)
  enable_memory_r();

  pr_id_ex.wbcs.RegDst = RegDst_rt;
  pr_id_ex.wbcs.RegWrite = RegWrite_yes;
  pr_id_ex.wbcs.MemToReg = MemToReg_memory_data;
}

inline void template_store_instruction()
{
  pr_id_ex.ecs.ALUSrc = ALUSrc_immediate;
  pr_id_ex.ecs.ALUOp = ALUOp_ADD;

  // We need to write the data.
  enable_memory_w();

  pr_id_ex.wbcs.RegWrite = RegWrite_no;
}

inline void template_itype_instruction()
{
  pr_id_ex.ecs.ALUSrc = ALUSrc_immediate;

  disable_memory();

  pr_id_ex.wbcs.RegDst = RegDst_rt;
  pr_id_ex.wbcs.RegWrite = RegWrite_yes;
  pr_id_ex.wbcs.MemToReg = MemToReg_ALU_result;
}

inline void template_branch_instruction()
{
  pr_id_ex.wbcs.PCSrc = PCSrc_branch;
  pr_id_ex.wbcs.RegWrite = RegWrite_no;
  pr_id_ex.ecs.ALUSrc = ALUSrc_rt;

  disable_memory();

  // Stall fetch we have to wait now.
  status.fetch = STATUS_STALL;

  // We won't be decoding as well, so we stall.
  status.decode = STATUS_STALL;
}

PIPE_LINE_STAGE void decode()
{
  // Reset control signals, so we only have to worry about setting true values.
  reset_control_signals();

  // Retrieve:
  // - PC
  // - Reg indicies
  // - Rt, Rs val
  // This is independent from instruction type.
  retrieve_values();


  // Set control signals
  enum EOPCODES instr = cast(enum EOPCODES, GET(OP, pr_if_id.instruction));

  switch (instr)
  {
    break; case REGIMM:
    {
      u8 op = GET_BLOCK(pr_if_id.instruction, 16, 5);

      switch (op)
      {
        break; case BLTZ:
        {
          template_branch_instruction();
          pr_id_ex.ecs.ALUOp = ALUOp_BLTZ;
        }   
        break; case BGEZ:
        {
          template_branch_instruction();
          pr_id_ex.ecs.ALUOp = ALUOp_BGEZ;
        }   
        break; case BLTZAL:
        {
          template_branch_instruction();

          link_next_pc();

          pr_id_ex.ecs.ALUOp = ALUOp_BLTZ;
        }   
        break; case BGEZAL:
        {
          template_branch_instruction();

          link_next_pc();

          pr_id_ex.ecs.ALUOp = ALUOp_BGEZ;
        }   
      }
    }

    break; case SPECIAL: // R Type Instructions
    {
      // Find out the ALUOp
      u8 code = GET(CD, pr_if_id.instruction);

      // We never need to write to memory
      disable_memory();

      // Execute
      pr_id_ex.ecs.ALUSrc = ALUSrc_rt;

      // Writeback
      pr_id_ex.wbcs.MemToReg = MemToReg_ALU_result;         
      pr_id_ex.wbcs.RegWrite = RegWrite_yes;        // We need to write to the register.

      // Register destination
      // The only reason I set it here like this and not inside handle_rtype() is so I don't forget.
      // Refactor: Remove this later.
      switch(code)
      {
      break; case SYSCALL:
      break; case JALR:
      break; case JR:
      break; case MTHI: pr_id_ex.wbcs.RegDst = RegDst_hi;
      break; case MTLO: pr_id_ex.wbcs.RegDst = RegDst_lo;
      break; default:   pr_id_ex.wbcs.RegDst = RegDst_rd; // Destination register is $rd.
      }

      handle_rtype(code);
    }

    break; case J:
    {
      dprint("Decoded: %s\n", "J");

      pr_id_ex.wbcs.PCSrc = PCSrc_jump; 

      pr_id_ex.ja = (pr_id_ex.pc & 0xffff0000) | (pr_id_ex.imm << 2);

      // Stall fetch and decode.
      status.fetch = STATUS_STALL;
      status.decode = STATUS_STALL;
    }

    break; case JAL:
    {
      dprint("Decoded: %s\n", "J");

      pr_id_ex.wbcs.PCSrc = PCSrc_jump; 

      pr_id_ex.ja = (pr_id_ex.pc & 0xffff0000) | (pr_id_ex.imm << 2);

      link_next_pc();

      // Stall fetch and decode.
      status.fetch = STATUS_STALL;
      status.decode = STATUS_STALL;
    }

    break; case BEQ:
    {
      dprint("Decoded: %s\n", "BEQ");
      template_branch_instruction();
      pr_id_ex.ecs.ALUOp = ALUOp_SUB;
    }
    break; case BNE:
    {
      dprint("Decoded: %s\n", "BNE");
      template_branch_instruction();

      // AH yes I love imaginary ALUs.
      pr_id_ex.ecs.ALUOp = ALUOp_BNE;
    }
    break; case BLEZ:
    {
      dprint("Decoded: %s\n", "BEQ");
      template_branch_instruction();
      pr_id_ex.ecs.ALUOp = ALUOp_BLEZ;
    }
    break; case BGTZ:
    {
      dprint("Decoded: %s\n", "BNE");
      template_branch_instruction();

      // AH yes I love imaginary ALUs.
      pr_id_ex.ecs.ALUOp = ALUOp_BGTZ;
    }

    break; case ADDIU:
           case ADDI:
    {
      dprint("Decoded: %s\n", "ADDI");
      template_itype_instruction();
      set_alu_op(ALUOp_ADD);
    }
    break; case ANDI:
    {
      // Zero extend
      pr_id_ex.imm &= 0xFFFF;
      template_itype_instruction();
      set_alu_op(ALUOp_AND);
    }
    break; case XORI:
    {
      // Zero extend
      pr_id_ex.imm &= 0xFFFF;
      template_itype_instruction();
      set_alu_op(ALUOp_XOR);
    }
    break; case SLTI:
    {
      // Zero extend
      template_itype_instruction();
      set_alu_op(ALUOp_SLT);
    }
    break; case SLTIU:
    {
      // Zero extend
      template_itype_instruction();
      set_alu_op(ALUOp_SLTU);
    }
    break; case ORI:
    {
      // Zero extend
      pr_id_ex.imm &= 0xFFFF;
      template_itype_instruction();
      set_alu_op(ALUOp_OR);
    }
    break; case LUI:
    {
      dprint("Decoded: %s\n", "LUI");
      template_itype_instruction();
      set_alu_op(ALUOp_LUI);
    }
    break; case LW: // Load word
    {
      dprint("Decoded: %s\n", "LW");
      template_load_instruction();
      pr_id_ex.mcs.LoadSz = LoadSz_word;
    }
    break; case LH: // Load half word
    {
      dprint("Decoded: %s\n", "LH");
      template_load_instruction();
      pr_id_ex.mcs.LoadSz = LoadSz_half;
      pr_id_ex.mcs.SignExtend = SignExtend_yes;
    }
    break; case LB: // Load half word
    {
      dprint("Decoded: %s\n", "LB");
      template_load_instruction();
      pr_id_ex.mcs.LoadSz = LoadSz_byte;
      pr_id_ex.mcs.SignExtend = SignExtend_yes;
    }
    break; case LHU: // Load half word
    {
      dprint("Decoded: %s\n", "LHU");
      template_load_instruction();
      pr_id_ex.mcs.LoadSz = LoadSz_half;
      pr_id_ex.mcs.SignExtend = SignExtend_no;
    }
    break; case LBU: // Load half word
    {
      dprint("Decoded: %s\n", "LBU");
      template_load_instruction();
      pr_id_ex.mcs.LoadSz = LoadSz_byte;
      pr_id_ex.mcs.SignExtend = SignExtend_no;
    }
    break; case SW:
    {
      dprint("Decoded: %s\n", "SW");
      template_store_instruction();
      pr_id_ex.mcs.LoadSz = LoadSz_word;
    }
    break; case SH:
    {
      dprint("Decoded: %s\n", "SH");
      template_store_instruction();
      pr_id_ex.mcs.LoadSz = LoadSz_half;
    }
    break; case SB:
    {
      dprint("Decoded: %s\n", "SB");
      template_store_instruction();
      pr_id_ex.mcs.LoadSz = LoadSz_byte;
    }
  }

  set_register_dependency();
}

inline void choose_register_destination()
{
  // RegDist:
  //  0: $rt
  //  1: $rd
  pr_ex_mem.rd = (pr_id_ex.wbcs.RegDst == 0) 
                  ? pr_id_ex.rti 
                  : pr_id_ex.rdi;

  switch (pr_id_ex.wbcs.RegDst)
  {
    break; case RegDst_hi: pr_ex_mem.rd = R_HI;
    break; case RegDst_lo: pr_ex_mem.rd = R_LO;
    break; case RegDst_rt: pr_ex_mem.rd = pr_id_ex.rti;
    break; case RegDst_rd: pr_ex_mem.rd = pr_id_ex.rdi;
  }
}

inline void calculate_pc_target()
{
  switch (pr_ex_mem.wbcs.PCSrc) 
  {
    break; case PCSrc_branch:
    {
      pr_ex_mem.target = pr_id_ex.pc + (pr_id_ex.imm * 4);
    }
    break; case PCSrc_jump:
    {
      dprint("PC: 0x%x\n", pr_id_ex.pc);
      pr_ex_mem.target = pr_id_ex.ja;
    }
  }
  // Default case is PC + 4.
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

void execute_alu()
{
  set_alu_result(0);
  switch (pr_id_ex.ecs.ALUOp)
  {
    break; case ALUOp_ADD: 
    {
      dprint("ALU ADD: %u + %u = %u\n", operand_a(), operand_b(), operand_a() + operand_b());
      set_alu_result(operand_a() + operand_b());
    }
    break; case ALUOp_SLL: 
    {
      set_alu_result(operand_b() << GET(SA, pr_id_ex.imm));
    }
    break; case ALUOp_SRL: 
    {
      set_alu_result(operand_b() >> GET(SA, pr_id_ex.imm));
    }
    break; case ALUOp_SLLV: 
    {
      set_alu_result(operand_b() << (operand_a() & MASK(5)));
    }
    break; case ALUOp_SRLV: 
    {
      set_alu_result(operand_b() >> (operand_a() & MASK(5)));
    }
    break; case ALUOp_SRA: 
    {
      dprint("ALU SRA%s\n", "");
      uint8_t sa = GET(SA, pr_id_ex.imm);
      dprint("SA: %u\n", sa);

      uint32_t operand = operand_b();
      uint32_t result = (cast(int, operand) >> sa);
      set_alu_result(result);
    }
    break; case ALUOp_SRAV: 
    {
      dprint("ALU SRAV%s\n", "");
      set_alu_result(cast(s32, operand_b()) >> (operand_a() & MASK(5)));
    }
    break; case ALUOp_LUI:
    {
      dprint("ALU LUI%s\n", "");
      set_alu_result(operand_b() << 16);
    }
    break; case ALUOp_SUB:
    {
      dprint("ALU SUB: %u - %u = %u\n", operand_a(), operand_b(), operand_a() - operand_b());
      set_alu_result(operand_a() - operand_b());
    }
    break; case ALUOp_OR:
    {
      set_alu_result(operand_a() | operand_b());
    }
    break; case ALUOp_SLT:
    {
      set_alu_result((cast(s32, operand_a()) < cast(s32, operand_b())) ? 1 : 0);
    }
    break; case ALUOp_SLTU:
    {
      set_alu_result((cast(u32, operand_a()) < cast(u32, operand_b())) ? 1 : 0);
    }
    break; case ALUOp_BNE:
    {
      bool different = (operand_a() != operand_b());
      set_alu_result(different ? 0 : 1);
      dprint("ALU BNE: 0x%x != 0x%x = %u\n", operand_a(), operand_b(), (different ? 0 : 1));
    }
    break; case ALUOp_BGTZ:
    {
      bool cond = (operand_a() != 0 && ((operand_a() >> 31) == 0));
      set_alu_result(cond ? 0 : 1);
    }
    break; case ALUOp_BLTZ:
    {
      bool cond = (operand_a() >> 31) == 1;
      set_alu_result(cond ? 0 : 1);
    }
    break; case ALUOp_BLEZ:
    {
      bool cond = (operand_a() == 0 || ((operand_a() >> 31) == 1));
      set_alu_result(cond ? 0 : 1);
    }
    break; case ALUOp_BGEZ:
    {
      bool cond = (operand_a() >> 31) == 0;
      set_alu_result(cond ? 0 : 1);
    }
    break; case ALUOp_AND:
    {
      set_alu_result(operand_a() & operand_b());
    }
    break; case ALUOp_XOR:
    {
      set_alu_result(operand_a() ^ operand_b());
    }
  }
}

PIPE_LINE_STAGE void execute()
{
  forward_control_id_ex_to_ex_mem();
  calculate_pc_target();
  // Forward the second register value (This is for SW)
  pr_ex_mem.rtv = pr_id_ex.rtv;
  choose_register_destination();
  execute_alu();
  pr_id_ex.ecs.ALUOp = ALUOp_NOOP;
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
  pr_mem_wb.target = pr_ex_mem.target;
  pr_mem_wb.wbcs = pr_ex_mem.wbcs;

  if (pr_ex_mem.mcs.MemRead) 
  {
    // The ALU computes the (base + offset)
    u32 data = mem_read_32(pr_ex_mem.alu_res);

    switch (pr_ex_mem.mcs.LoadSz) 
    {
      break; case LoadSz_half:
      {
        data = GET_BLOCK(data, 0, WORD);

        if (pr_ex_mem.mcs.SignExtend == SignExtend_yes)
        {
          data = sign_extend_16(data);
        }
      }
      break; case LoadSz_byte:
      {
        data = GET_BLOCK(data, 0, BYTE);

        if (pr_ex_mem.mcs.SignExtend == SignExtend_yes)
        {
          data = sign_extend_8(data);
        }
      }
    }
    pr_ex_mem.mem_res = data;
    dprint("Loaded: 0x%x\n", pr_ex_mem.mem_res);
  }

  if (pr_ex_mem.mcs.MemWrite)
  {
    u32 data = pr_ex_mem.rtv;

    switch (pr_ex_mem.mcs.LoadSz) 
    {
      break; case LoadSz_word:
      {
        mem_write_32(pr_ex_mem.alu_res, data);
      }
      break; case LoadSz_half:
      {
        data = GET_BLOCK(data, 0, WORD);
        mem_write_32(pr_ex_mem.alu_res, data);
      }
      break; case LoadSz_byte:
      {
        data = GET_BLOCK(data, 0, BYTE);
        mem_write_32(pr_ex_mem.alu_res, data);
      }
    }
    dprint("Stored: %u at 0x%x\n", data, pr_ex_mem.alu_res);
  }

  if (pr_ex_mem.mcs.MemRead == MemRead_no && pr_ex_mem.mcs.MemWrite == MemWrite_no)
  {
    dprint("Nothing done%s\n", "");
  }

  // EX_WriteRegister -> MEM_WriteRegister
  pr_mem_wb.rd = pr_ex_mem.rd;

  // EX_ALUResult -> MEM_ALUResult
  pr_mem_wb.alu_res = pr_ex_mem.alu_res;

  // EX_MemResult -> MEM_MemResult
  pr_mem_wb.mem_res = pr_ex_mem.mem_res;

}


inline u32 write_back_data()
{
  if (pr_mem_wb.wbcs.MemToReg == MemToReg_memory_data)
  {
    // TODO: return the load data...
    dprint("returing write back data: %d\n", pr_mem_wb.mem_res);
    return pr_mem_wb.mem_res;
  }
  else
  {
    return pr_mem_wb.alu_res;
  }
}


inline void set_register_ready()
{
  if (status.decode == STATUS_STALL && fetch_count > decode_count) // if decode is stalling, we can now set it to ready.
  {
    status.decode = STATUS_READY;
  }

  REG_STATUS[pr_mem_wb.rd] = true;
}

PIPE_LINE_STAGE void writeback()
{
  if (pr_mem_wb.wbcs.RegWrite == RegWrite_yes)
  {
    dprint("Writing 0x%x to register %u\n", write_back_data(), pr_mem_wb.rd);

    switch (pr_mem_wb.rd)
    {
      break; case R_HI: pr_ex_mem.rd = CURRENT_STATE.HI = write_back_data();
      break; case R_LO: pr_ex_mem.rd = CURRENT_STATE.LO = write_back_data();
      break; default: CURRENT_STATE.REGS[pr_mem_wb.rd] = write_back_data();
    }
    
    set_register_ready();
  }

  if (pr_mem_wb.wbcs.PCSrc != PCSrc_normal)
  {
    // We got jump/branch.

    if (pr_mem_wb.wbcs.PCSrc == PCSrc_jump || pr_mem_wb.alu_res == 0)
    {
      // This is a branching instruction and we have to take it.
      dprint("Branch taken, now at 0x%x!\n", pr_mem_wb.target);
      CURRENT_STATE.PC = pr_mem_wb.target;
    }
    else
    {
      dprint("Branch not taken%s!\n", "");
    }

    // This can be set whether branch is taken or not.
    // So it has to be in the outside scope.
    REG_STATUS[R_RA] = REG_READY;

    // We can safely resume fetch now.
    status.fetch = STATUS_READY;
  }
}

#ifdef ALLOW_PIPELINE_HISTORY
#define log_history(stage, ch) \
pipeline_history[HISTORY_LINE_LENGTH * stage##_count + cycle_number] = (status.stage == STATUS_READY) ? ch : '-'
#define log_fetch(...) pipeline_history[HISTORY_LINE_LENGTH * fetch_count + cycle_number] = 'f';
#else
#define log_history(...)
#define log_fetch(...)
#endif

void process_instruction() 
{
  log_history(writeback, 'w');
  log_history(memory, 'm');
  log_history(execute, 'e');

  if (status.writeback == STATUS_READY)
  {
    writeback_count++;
    writeback();
    status.writeback = STATUS_STALL;
  }

  if (status.memory == STATUS_READY)
  {
    memory_count++;
    status.writeback = STATUS_READY;
    memory();
    status.memory = STATUS_STALL;
  }

  if (status.execute == STATUS_READY)
  {
    execute_count++;
    status.memory = STATUS_READY;
    execute();
    status.execute = STATUS_STALL;
  }

  if (status.decode == STATUS_READY)
  {
    status.execute = STATUS_READY;

    // Check if decode is causes data hazard.
    if (has_dependency())
    {
      // Avoid overwriting!!
      // We are stalling decode so we also have to stall fetch!
      status.fetch = STATUS_STALL;

      // This can only be reactivated during the writeback stage. (No data forward)
      status.decode = STATUS_STALL;

      status.execute = STATUS_STALL;

      log_history(decode, 'd');
    }
    else
    {
      log_history(decode, 'd');
      decode_count++;
      status.fetch = STATUS_READY;
      decode();
      status.decode = STATUS_STALL;
    }
  }

  if (status.fetch == STATUS_READY)
  {
    log_fetch();
    fetch_count++;
    status.decode = STATUS_READY;
    fetch();
  }

  cycle_number++;
}

void lprocess_instruction() {
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
