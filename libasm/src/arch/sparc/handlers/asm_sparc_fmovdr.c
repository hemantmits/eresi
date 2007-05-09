/*
**
** $Id: asm_sparc_fmovdr.c,v 1.4 2007-04-22 20:48:41 strauss Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fmovdr(asm_instr * ins, u_char * buf, u_int len,
		 asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);
  
  ins->type = ASM_TYPE_ASSIGN | ASM_TYPE_TEST;

  inter = proc->internals;

  ins->instr = inter->fmovr_table[(((opcode.opf & 0x1f) - 6) * 8)
				  + opcode.rcond];
  ins->nb_op = 3;
  ins->op1.type = ASM_SP_OTYPE_FREGISTER;
  ins->op1.base_reg = opcode.rd;
  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  ins->op2.base_reg = opcode.rs2;
  ins->op3.type = ASM_SP_OTYPE_REGISTER;
  ins->op3.base_reg = opcode.rs1;
  
  return 4;
}