/*
**
** $Id: asm_sparc_lddf.c,v 1.4 2007-04-22 20:48:41 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_lddf(asm_instr * ins, u_char * buf, u_int len,
	       asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_LOAD | ASM_TYPE_ASSIGN;

  ins->nb_op = 2;
  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  ins->op1.base_reg = opcode.rd;

  if (opcode.i) {
    ins->op2.type = ASM_SP_OTYPE_IMM_ADDRESS;
    ins->op2.imm = opcode.imm;
  }
  else {
    ins->op2.type = ASM_SP_OTYPE_REG_ADDRESS;
    ins->op2.index_reg = opcode.rs2;
  }
  ins->op2.base_reg = opcode.rs1;
  
  return 4;

}