#include <stdio.h>
#include "shell.h"
#include "utils.h"

// REGISTERS
#define R_V0 2

#define SPECIAL_OP ((uint32_t)0x00)
#define ADDI  ((uint32_t)0x08)
#define ADDIU ((uint32_t)0x09)

#define INSTR_SIZE 32
#define OP_SIZE 6
#define RS_SIZE 5
#define RT_SIZE 5
#define IM_SIZE 16

// creates a mask of n 1s
#define MASK(n) (~((~((uint32_t)0)) << n))
#define MASK1(n, p) (MASK(n)<<p)
#define MASK0(n, p) (~(MASK1))

#define OP_POS (INSTR_SIZE-OP_SIZE)
#define RS_POS (OP_POS-RS_SIZE)
#define RT_POS (RS_POS-RT_SIZE)
#define IM_POS (RT_POS-IM_SIZE)

#define GET_OP(addr) (addr>>OP_POS) & MASK(6)
#define GET_RS(addr) (addr>>RS_POS) & MASK(5)
#define GET_RT(addr) (addr>>RT_POS) & MASK(5)
#define GET_IM(addr) (addr>>IM_POS) & MASK(16)

/////////////////////////////////////
// NOTE(Appy): Uninspiring helpers

#define PASS_DOWN(OP) case OP

#define HANDLE(OP) \
  case OP: \
  { \
    instr_ ## OP(mem); break; \
  }

#define HANDLER(OP) void instr_##OP(uint32_t mem) 

#define DISPATCH(code) switch(code)


/////////////////////////////////////
// NOTE(Appy): Handlers

HANDLER(ADDIU)
{
  uint8_t  rs    = GET_RS(mem);
  uint8_t  rt    = GET_RT(mem);
  uint16_t imm   = GET_IM(mem);
  int result = ((uint32_t)imm);
  if (result & MASK1(1, 15)) { result |= MASK1(16,16); }
  NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
}

HANDLER(SPECIAL_OP)
{
  if (CURRENT_STATE.REGS[R_V0] == 0x0A)
  {
    RUN_BIT = FALSE;
    return;
  }  
}

void process_instruction()
{
  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory. */
  uint32_t mem = mem_read_32(CURRENT_STATE.PC);
  uint8_t instr = GET_OP(mem);

  DISPATCH(instr)
  {
    // Handle special 
    HANDLE(SPECIAL_OP) 

    // Handle add
    PASS_DOWN(ADDI):
    HANDLE(ADDIU)
  }
  NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
