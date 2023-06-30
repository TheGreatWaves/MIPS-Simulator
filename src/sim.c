#include <stdio.h>
#include "shell.h"
#include "utils.h"

#define CAT(x, s) x ## s
#define CAT_HELPER(x, s) CAT(x, s)
#define PADDING CAT_HELPER(PAD, __COUNTER__)

static int jump_pending = -1;

/////////////////////////////////////
// NOTE(Appy): Opcodes automation
#define OPCODES(x) \
  x(SPECIAL)       \
  x(PADDING)       \
  x(J)             \
  x(JAL)           \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(ADDIU)         \
  x(ADDI)          \
  x(SLTI)          \
  x(SLTIU)         \
  x(ANDI)          \
  x(ORI)           \
  x(XORI)          \
  x(LUI)           \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(SB)            \
  x(SH)            \
  x(PADDING)       \
  x(SW)            \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \

#define ENUMERATE(OP) OP,
enum EOPCODES { OPCODES(ENUMERATE) NUMBER_OF_OPS };


/////////////////////////////////////
// NOTE(Appy): Registers

#define R_V0 2

/////////////////////////////////////
// NOTE(Appy): Cast

#define u32t(V)  (cast(uint32_t, V))

/////////////////////////////////////
// NOTE(Appy): R-Type instructions

#define SLL     u32t(0x00)
#define SRL     u32t(0x02)
#define SRA     u32t(0x03)
#define JR      u32t(0x8)
#define MTHI    u32t(0x11)
#define MTLO    u32t(0x13)
#define MULT    u32t(0x18)
#define ADD     u32t(0x20)
#define ADDU    u32t(0x21)
#define SUB     u32t(0x22)
#define AND     u32t(0x24)
#define SUBU    u32t(0x23)
#define OR      u32t(0x25)
#define XOR     u32t(0x26)
#define NOR     u32t(0x27)
#define DIV     u32t(0x1A)
#define DIVU    u32t(0x1B)
#define SYSCALL u32t(0xC)

/////////////////////////////////////
// NOTE(Appy): Segment sizes

#define INSTR_SIZE 32
#define OP_SIZE     6 
#define RS_SIZE     5 
#define RT_SIZE     5
#define RD_SIZE     5
#define SA_SIZE     5
#define IM_SIZE    16
#define CD_SIZE     6
#define SHAMT_SIZE  5

/////////////////////////////////////
// NOTE(Appy): Segment positions
//             
// RS    - Operand A in register file
// RT    - Operand B in register file
// RD    - Result destination in register file
// SHAMT - Shift amount
// CD    - Function code

#define OP_POS    (INSTR_SIZE-OP_SIZE)
#define RS_POS    (OP_POS-RS_SIZE)
#define RT_POS    (RS_POS-RT_SIZE)
#define RD_POS    (RT_POS-RD_SIZE)
#define SA_POS    (RD_POS-SA_SIZE)
#define IM_POS    (RT_POS-IM_SIZE)
#define CD_POS    (0)
#define SHAMT_POS (6)

/////////////////////////////////////
// NOTE(Appy): Segment getters

#define GET_BLOCK(addr, start, size) ((addr>>(start)) & MASK(size))
#define GET(SEG, addr) GET_BLOCK(addr, SEG##_POS, SEG##_SIZE)

/////////////////////////////////////
// NOTE(Appy): Uninspiring helpers

#define PASS_DOWN(OP) case OP
#define HANDLE(OP) case OP :{ instr_ ## OP(mem); break; }
#define HANDLER(OP) void instr_##OP(uint32_t mem) 
#define CALL_HANDLER(OP) instr_##OP(mem) 
#define FORWARD_HANDLER(OP, TO) HANDLER(OP) { CALL_HANDLER(TO); }
#define DISPATCH(code) goto *jumpTable[ code ];
#define NEXT goto *jumpTable[ u32t(NUMBER_OF_OPS) ]
#define JUMPTABLE static const void *jumpTable[]
#define MK_LBL(OP) &&lbl_##OP,
#define LBL(OP) lbl_##OP

/////////////////////////////////////
// NOTE(Appy): Handlers

HANDLER(SPECIAL)
{
  uint8_t code = GET(CD, mem);

  /* Undefine register helpers from process instruction */
  #undef RS 
  #undef RT 
  #undef IMM 

  /* Register helpers */
  #define RD (NEXT_STATE.REGS[ GET(RD, mem) ])
  #define RS (CURRENT_STATE.REGS[ GET(RS, mem) ])
  #define RT (CURRENT_STATE.REGS[ GET(RT, mem) ])
  #define SA (GET(SA, mem))

  switch(code)
  {
    case SYSCALL:                        // System call
    {     
      if (CURRENT_STATE.REGS[R_V0] == 0x0A)
      {
        RUN_BIT = FALSE;
      }  
      /* else do nothing... increment as usual */
      break;
    }
    case ADDU: 
    case ADD:  { RD = RS + RT; break; }  // Addition
    case SUB:  { RD = RS - RT; break; }  // Subtraction
    case OR:   { RD = RS | RT; break; }  // Bit Or
    case AND:  { RD = RS & RT; break; }  // Bit And
    case SUBU: { RD = RS - RT; break; }  // Subtraction Unsigned
    case XOR:  { RD = RS ^ RT; break; }  // Exclusive Or
    case NOR:  { RD = NOR_OP(RS,  RT); break; }  // Nor
    case JR:
    {
      // Load jump, horrible hack, starts next one at RS
      CURRENT_STATE.PC = RS - 4;
      break;
    }
    case MTLO:
    {
        CURRENT_STATE.LO = RS;
        break;
    }
    case MTHI:
    {
        CURRENT_STATE.HI = RS;
        break;
    }
    case MULT:
    {
        u64 result = cast(u64, RS) * cast(u64, RT);
        CURRENT_STATE.HI = (~(cast(u32, 0))) & (result >> 32);
        CURRENT_STATE.LO = (~(cast(u32, 0))) & result;
        break;
    }
    case DIV:  
    {
      s32 denominator = TWOCOMP(RT);
      s32 numerator = TWOCOMP(RS);
      if (denominator != 0)
      {
        NEXT_STATE.LO = u32t(numerator/denominator);
        NEXT_STATE.HI = u32t(numerator % denominator);
      }
      break;
    }
    case DIVU:  
    {
      u32 denominator = u32t(RT);
      u32 numerator = u32t(RS);
      if (denominator != 0)
      {
        NEXT_STATE.LO = u32t(numerator/denominator);
        NEXT_STATE.HI = numerator % denominator;
      }
      break;
    }
    case SLL:  { RD = RT << SA; break; } // Shift Left
    case SRL:  { RD = RT >> SA; break; } // Shirt Right
    case SRA:                            // Shift Right Addition
    {
      uint8_t  sa      = SA;
      uint32_t operand = RT;
      uint32_t result  = (operand >> sa);

      /* Sign extension */
      result = sign_extend(result);
        
      RD = result;
      break;
    }
  }

  #undef RD
  #undef RS
  #undef RT
  #undef SA
}

void process_instruction()
{
  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory. */
  uint32_t mem = mem_read_32(CURRENT_STATE.PC);
  uint8_t instr = GET(OP, mem);

  /* Instruction jump tables */
  static const void *jumpTable[] = { OPCODES(MK_LBL) MK_LBL(NEXT_STATE) };

  /* Helpers */
  #define END_LABEL LBL(NEXT_STATE): 
  #define RS  (CURRENT_STATE.REGS[GET(RS, mem)])
  #define RT  (NEXT_STATE.REGS[GET(RT, mem)])
  #define IMM (GET(IM, mem))

  /* Dispatch the instruction. */
  DISPATCH(instr) 
  {
    /* First six bits are 0s */
    LBL(SPECIAL): { CALL_HANDLER(SPECIAL); NEXT; }

    /* Basic cases, none of these messes control flow */
    LBL(ADDI):
    LBL(ADDIU):
    {
      uint32_t result = u32t(IMM);

      // TODO(Appy): Macro abuse this?
      /* Bit extend. */
      if (result & MASK1(1, 15)) { result |= MASK1(16,16); }
      RT = RS + result;
      NEXT;
    }
    LBL(XORI): { RT = RS ^ u32t(IMM); NEXT; }    
    LBL(ANDI): { RT = RS & u32t(IMM); NEXT; }
    LBL(ORI): { RT = RS | u32t(IMM); NEXT; }
    LBL(LUI):  { RT = (IMM << 16); NEXT; }
    LBL(J): 
    {
      u32 jp = GET_BLOCK(mem, 25, 26);
      NEXT_STATE.PC = (CURRENT_STATE.PC | (jp << 2))-4;
      NEXT;
    }

    /* Default case. */
    LBL(PADDING):  { NEXT; }
    LBL(SB): {
      uint32_t offset = sign_extend_16(IMM);
      uint32_t address = offset + RS;
      uint32_t last_byte = u32t(GET_BLOCK(address, 0, 8));
      mem_write_32(address, last_byte);
      NEXT;
    }
    LBL(SH): {
      uint32_t offset = sign_extend_16(IMM);
      uint32_t address = offset + RS;
      uint32_t last_byte = u32t(GET_BLOCK(address, 0, 16));
      mem_write_32(address, last_byte);
      NEXT;
    }
    LBL(SLTI): 
    {
      s32 result = RS - cast(s32, sign_extend_16(IMM));
      RT = (cast(s32, RS) < result) ? 1 : 0;
      NEXT;
    }
    LBL(SLTIU): 
    {
      u32 result = RS - sign_extend_16(IMM);
      RT = (RS < result) ? 1 : 0;
      NEXT;
    }
    LBL(SW):
    {
      u32 vAddr = RS + sign_extend_16(IMM);
      mem_write_32(vAddr, RT);
      NEXT;
    }
    LBL(JAL):
    {
      u32 temp = (GET_BLOCK(mem, 0, 25) << 2);
      NEXT;
    }
  }

  /* This is place in this way in order to allow disabling of threaded code */
  END_LABEL 
  {
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
