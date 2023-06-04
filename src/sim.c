#include <stdint.h>
#include <stdio.h>
#include "shell.h"
#include "utils.h"

#define CAT(x, s) x ## s
#define CAT_HELPER(x, s) CAT(x, s)
#define PADDING CAT_HELPER(PAD, __COUNTER__)

/////////////////////////////////////
// NOTE(Appy): Opcodes automation
#define OPCODES(x) \
  x(SPECIAL)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(ADDIU)         \
  x(ADDI)          \
  x(PADDING)       \
  x(PADDING)       \
  x(ANDI)          \
  x(PADDING)       \
  x(XORI)          \
  x(LUI)           \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       

#define ENUMERATE(OP) OP,
enum EOPCODES { OPCODES(ENUMERATE) NUMBER_OF_OPS };

/////////////////////////////////////
// NOTE(Appy): Utils

#define MASK(n) (~((~((uint32_t)0)) << n)) // creates a mask of n 1s
#define MASK1(n, p) (MASK(n)<<(p))
#define MASK0(n, p) (~(MASK1))

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
#define ADD     u32t(0x20)
#define ADDU    u32t(0x21)
#define SUB     u32t(0x22)
#define AND     u32t(0x24)
#define SUBU    u32t(0x23)
#define OR      u32t(0x25)
#define XOR     u32t(0x26)
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

HANDLER(ADDIU)
{
  uint8_t  rs  = GET(RS, mem);
  uint8_t  rt  = GET(RT, mem);
  uint16_t imm = GET(IM, mem);
  int result = u32t(imm);

  /* Bit extend. */
  if (result & MASK1(1, 15)) { result |= MASK1(16,16); }
  NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
}

/* Probably should not */
FORWARD_HANDLER(ADDI, ADDIU);

HANDLER(XORI)
{
  uint8_t  rs  = GET(RS, mem);
  uint8_t  rt  = GET(RT, mem);
  uint16_t imm = GET(IM, mem);
  NEXT_STATE.REGS[rt] = u32t(CURRENT_STATE.REGS[rs]) ^ u32t(imm);
}

HANDLER(ANDI)
{
  uint8_t  rs  = GET(RS, mem);
  uint8_t  rt  = GET(RT, mem);
  uint16_t imm = GET(IM, mem);
  NEXT_STATE.REGS[rt] = u32t(CURRENT_STATE.REGS[rs]) & u32t(imm);
}

/** 
 * Load Upper Immediate (15)
 *
 * The 16-bit immediate is shifted left by 16 bits.
 * The result is placed into the register rt.
 */
HANDLER(LUI)
{
  uint8_t  rt  = GET(RT, mem);
  uint16_t imm = GET(IM, mem);
  NEXT_STATE.REGS[rt] = (imm << 16);
}

HANDLER(SPECIAL)
{
  /* Maybe not ideal. */
  uint8_t code = GET(CD, mem);

  // Note(Appy): Sorry for hacks
  #define APPLY(dest, a, op, b) \
  NEXT_STATE.REGS[ GET(dest, mem) ] = CURRENT_STATE.REGS[ GET(a, mem) ] op CURRENT_STATE.REGS[ GET(b, mem) ]

  switch(code)
  {
    case SYSCALL:
    {     
      if (CURRENT_STATE.REGS[R_V0] == 0x0A)
      {
        RUN_BIT = FALSE;
      }  
      break;
    }
    case ADDU: 
    case ADD:  { APPLY(RD, RS, +, RT); break; }
    case SUB:  { APPLY(RD, RS, -, RT); break; }
    case OR:   { APPLY(RD, RS, |, RT); break; }
    case AND:  { APPLY(RD, RS, &, RT); break; }
    case SUBU: { APPLY(RD, RS, -, RT); break; }
    case XOR:  { APPLY(RD, RS, ^, RT); break; }
    case SLL:
    {
      uint8_t rt   = GET(RT, mem);
      uint8_t rd   = GET(RD, mem);
      uint8_t sa   = GET(SA, mem);
      NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
      break;
    }
    case SRL:
    {
      uint8_t rt   = GET(RT, mem);
      uint8_t rd   = GET(RD, mem);
      uint8_t sa   = GET(SA, mem);
      NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
      break;
    }
    case SRA:
    {
      uint8_t rt       = GET(RT, mem);
      uint8_t rd       = GET(RD, mem);
      uint8_t sa       = GET(SA, mem);
      uint32_t operand = CURRENT_STATE.REGS[rt];
      uint32_t result  = operand >> sa;

      /* Sign extension */
      if (operand & MASK1(1, 31))
      {
         result |= MASK1(sa, 32-sa); 
      }
        
      NEXT_STATE.REGS[rd] = result;
      break;
    }
  }
  #undef APPLY
}

void process_instruction()
{
  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory. */
  uint32_t mem = mem_read_32(CURRENT_STATE.PC);
  uint8_t instr = GET(OP, mem);

  static const void *jumpTable[] = { OPCODES(MK_LBL) MK_LBL(NEXT_STATE) };

  #define HANDLE_BASIC(OP) LBL(OP):{CALL_HANDLER(OP);NEXT;}

  /* Dispatch the instruction. */
  DISPATCH(instr) 
  {
    /* Basic cases, none of these messes control flow */
    HANDLE_BASIC(ADDI);
    HANDLE_BASIC(ADDIU);
    HANDLE_BASIC(SPECIAL);
    HANDLE_BASIC(XORI);
    HANDLE_BASIC(ANDI);
    HANDLE_BASIC(LUI);

    /* Default case. */
    LBL(PADDING):  { NEXT; }
  }

  LBL(NEXT_STATE): { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }
}
