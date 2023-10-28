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
#include <stdbool.h>
#include "utils.h"

#define FALSE 0
#define TRUE  1

#define MIPS_REGS 32

// #define ALLOW_PIPELINE_HISTORY

// Assume max cycle is 100 (Too much to fit on screen)
#define MAX_CYCLES 5000
#define HISTORY_LINE_LENGTH MAX_CYCLES
#define HISTORY_MAX_LINES 20
#define HISTORY_BUFFER_SIZE HISTORY_LINE_LENGTH * HISTORY_MAX_LINES
extern char pipeline_history[HISTORY_BUFFER_SIZE];
extern u32 fetch_count;
extern u32 decode_count;
extern u32 execute_count;
extern u32 memory_count;
extern u32 writeback_count;

extern u32 alu_result_to_forward;

extern u32 cycles_padding;

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


/////////////////////////////////////
// NOTE(Appy): Labels for organization
#define PIPE_LINE_STAGE
#define PIPE_LINE_REGISTER

/////////////////////////////////////
// NOTE(Appy): Registers

#define R_V0 2
#define R_RA 31
#define R_LO 32
#define R_HI 33

/////////////////////////////////////
// NOTE(Appy): Control Signals

typedef struct {
  u32 ALUSrc: 1;
  u32 ALUOp:  6;
} ExecuteControlSignals;

typedef struct {
  u32 MemRead:    1;
  u32 MemWrite:   1;
  u32 LoadSz:     2;
  u32 SignExtend: 1;
} MemoryControlSignals;

typedef struct {
  u32 RegWrite: 1;
  u32 RegDst:   2;
  u32 MemToReg: 1;
  u32 PCSrc:    2; 
} WriteBackControlSignals;

/////////////////////////////////////
// NOTE(Appy): Pipeline stalls

typedef struct {
  u32 fetch:     1; 
  u32 decode:    1;
  u32 execute:   1;
  u32 memory:    1;
  u32 writeback: 1;
} Stall;

#define STATUS_STALL 1
#define STATUS_READY 0

/////////////////////////////////////
// NOTE(Appy): Dependency detection

#define REG_NOT_READY false
#define REG_READY true

extern bool REG_STATUS[MIPS_REGS+2];

/////////////////////////////////////
// NOTE(Appy): Pipeline Registers

/* IF/ID */
PIPE_LINE_REGISTER typedef struct {
  u32 instruction;
  u32 pc;
} PR_IF_ID;

/* ID/EX */
PIPE_LINE_REGISTER typedef struct {
  /* Data */
  u32 pc;   // Program counter
  u32 imm;  // Sign extended immediate
  u32 ja;   // Jump address

  /* Register index */
  u8  rsi;  
  u8  rti;  
  u8  rdi;

  /* Register values */
  u32 rsv;   // REGS[$RS] 
  u32 rtv;   // REGS[$RT]

  u8 forwarded;

  /* Control Signals */
  ExecuteControlSignals   ecs;
  MemoryControlSignals    mcs;
  WriteBackControlSignals wbcs;
} PR_ID_EX;

PIPE_LINE_REGISTER typedef struct 
{
  u32                     alu_res;
  u32                     mem_res;
  u32                     target;
  u8                      rd;       // EX_WriteRegister
  u32                     rtv;      // REGS[$RT]
  MemoryControlSignals    mcs;
  WriteBackControlSignals wbcs;
  bool                    branch;
} PR_EX_MEM;

PIPE_LINE_REGISTER typedef struct 
{
  u32                     memory_read;
  u32                     alu_res;
  u32                     mem_res;
  u32                     target;
  u8                      rd;
  bool                    read_mem;
  WriteBackControlSignals wbcs;
} PR_MEM_WB;

extern PR_IF_ID pr_if_id;
extern PR_ID_EX pr_id_ex;
extern PR_EX_MEM pr_ex_mem;
extern PR_MEM_WB pr_mem_wb;

void reset_control_signals();
void reset_stall();
void reset_history();

extern u32 cycle_number;

extern Stall status;

#endif
