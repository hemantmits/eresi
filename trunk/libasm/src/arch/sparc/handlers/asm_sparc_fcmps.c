/*
**
** $Id: asm_sparc_fcmps.c,v 1.6 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libasm.h"

int
asm_sparc_fcmps(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_TEST;

  ins->instr = inter->fcmp_table[(opcode.opf & 0x1f) - 16];
  ins->nb_op = 3;
  asm_sparc_op_fetch(&ins->op1, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op1.baser = opcode.rs2;
  asm_sparc_op_fetch(&ins->op2, buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op2.baser = opcode.rs1;
  asm_sparc_op_fetch(&ins->op3, buf, ASM_SP_OTYPE_CC, ins);
  ins->op3.baser = opcode.cc;
  
  return 4;
}
