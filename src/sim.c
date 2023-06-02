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
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
  x(PADDING)       \
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
#define MASK1(n, p) (MASK(n)<<p)
#define MASK0(n, p) (~(MASK1))

/////////////////////////////////////
// NOTE(Appy): Registers

#define R_V0 2

/////////////////////////////////////
// NOTE(Appy): Instructions

#define u32t(V)  (cast(uint32_t, V))
#define ISPECIAL u32t(0x00)
#define IADDI    u32t(0x08)
#define IADDIU   u32t(0x09)


/////////////////////////////////////
// NOTE(Appy): Special instructions

#define SYSCALL u32t(0xC)
#define ADD     u32t(0x20)
#define OR      u32t(0x25)

/////////////////////////////////////
// NOTE(Appy): Segment sizes

#define INSTR_SIZE 32
#define OP_SIZE 6
#define RS_SIZE 5
#define RT_SIZE 5
#define RD_SIZE 5
#define IM_SIZE 16
#define CD_SIZE 6

/////////////////////////////////////
// NOTE(Appy): Segment positions

#define OP_POS (INSTR_SIZE-OP_SIZE)
#define RS_POS (OP_POS-RS_SIZE)
#define RT_POS (RS_POS-RT_SIZE)
#define RD_POS (RT_POS-RD_SIZE)
#define IM_POS (RT_POS-IM_SIZE)
#define CD_POS (0)

/////////////////////////////////////
// NOTE(Appy): Segment getters

#define GET_BLOCK(addr, start, size) ((addr>>(start)) & MASK(size))

#define GET_OP(addr) GET_BLOCK(addr, OP_POS, OP_SIZE)
#define GET_RS(addr) GET_BLOCK(addr, RS_POS, RS_SIZE)
#define GET_RT(addr) GET_BLOCK(addr, RT_POS, RT_SIZE)
#define GET_RD(addr) GET_BLOCK(addr, RD_POS, RD_SIZE)
#define GET_IM(addr) GET_BLOCK(addr, IM_POS, IM_SIZE)

/////////////////////////////////////
// NOTE(Appy): Uninspiring helpers

#define PASS_DOWN(OP) case OP
#define HANDLE(OP) case OP :{ instr_ ## OP(mem); break; }
#define HANDLER(OP) void instr_##OP(uint32_t mem) 
#define CALL_HANDLER(OP) instr_##OP(mem) 
#define DISPATCH(code) goto *jumpTable[ code ];
#define NEXT goto *jumpTable[ cast(u32, NUMBER_OF_OPS) ]
#define JUMPTABLE static const void *jumpTable[]
#define MK_LBL(OP) &&lbl_##OP,
#define LBL(OP) lbl_##OP

/////////////////////////////////////
// NOTE(Appy): Handlers

HANDLER(IADDIU)
{
  uint8_t  rs    = GET_RS(mem);
  uint8_t  rt    = GET_RT(mem);
  uint16_t imm   = GET_IM(mem);
  int result = ((uint32_t)imm);
  if (result & MASK1(1, 15)) { result |= MASK1(16,16); }
  gprint(CURRENT_STATE.REGS[rs] + imm);
  NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
}

HANDLER(ISPECIAL)
{
  uint8_t code = GET_BLOCK(mem, CD_POS, CD_SIZE);
  switch(code)
  {
    case SYSCALL:
    {     
      if (CURRENT_STATE.REGS[R_V0] == 0x0A)
      {
        RUN_BIT = FALSE;
        break;
      }  
    }
    case ADD:
    {
      uint8_t rs = GET_RS(mem);
      uint8_t rt = GET_RT(mem);
      uint8_t rd = GET_RD(mem);
      NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
      break;
    }
    case OR:
    {
      gprint("HELLO");
    }
  }
}

HANDLER(IADDI) { CALL_HANDLER(IADDIU); }

void process_instruction()
{
  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory. */
  uint32_t mem = mem_read_32(CURRENT_STATE.PC);
  uint8_t instr = GET_OP(mem);

  static const void *jumpTable[] = { OPCODES(MK_LBL) MK_LBL(NEXT_STATE) };

  /* Dispatch the instruction. */
  DISPATCH(instr) 
  {
    LBL(ADDI):
    LBL(ADDIU):
    {
      CALL_HANDLER(IADDIU);
      NEXT;
    }
    LBL(SPECIAL):
    {
      CALL_HANDLER(ISPECIAL);
      NEXT;
    }
    LBL(PADDING): {
      NEXT;
    }
  }

  LBL(NEXT_STATE): { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }
}
