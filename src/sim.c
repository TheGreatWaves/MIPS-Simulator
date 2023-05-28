#include <stdio.h>
#include "shell.h"

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

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
  uint32_t mem = mem_read_32(CURRENT_STATE.PC);

  uint8_t  instr = GET_OP(mem);
  uint8_t  rs    = GET_RS(mem);
  uint8_t  rt    = GET_RT(mem);
  uint16_t imm   = GET_IM(mem);

  // printf("instruction: %d\n", instr);
  // printf("instruction: %d\n", rs);
  // printf("instruction: %d\n", rt);
  // printf("instruction: %d\n", imm);

  int result = ((uint32_t)imm);
  if (result & MASK1(1, 15))
  {
    result |= MASK1(16,16);
  }

  printf("result: %d\n", imm);
  CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
  
  exit(0);
}
