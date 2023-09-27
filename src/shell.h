/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   For TGGS Computer Architecture                            */
/*                                                             */
/*   Credit: ECE 447                                           */
/*   Carnegie Mellon University                                */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change sim.c!                       */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifndef _SIM_SHELL_H_
#define _SIM_SHELL_H_

#include <stdint.h>
#include "utils.h"

#define FALSE 0
#define TRUE  1

#define MIPS_REGS 32

// Assume max cycle is 100 (Too much to fit on screen)
#define HISTORY_LINE_LENGTH 40
#define HISTORY_MAX_LINES 10
#define HISTORY_BUFFER_SIZE HISTORY_LINE_LENGTH * HISTORY_MAX_LINES
extern char pipeline_history[HISTORY_BUFFER_SIZE];
extern u32 fetch_count;
extern u32 decode_count;
extern u32 execute_count;
extern u32 memory_count;
extern u32 writeback_count;

typedef struct CPU_State_Struct {

  uint32_t PC;		/* program counter */
  uint32_t REGS[MIPS_REGS]; /* register file. */
  uint32_t HI, LO;          /* special regs for mult/div. */
} CPU_State;

/* Data Structure for Latch */

extern CPU_State CURRENT_STATE, NEXT_STATE;

extern int RUN_BIT;	/* run bit */

uint32_t mem_read_32(uint32_t address);
void     mem_write_32(uint32_t address, uint32_t value);

/* YOU IMPLEMENT THIS FUNCTION */
void process_instruction();

void reset();

#endif
