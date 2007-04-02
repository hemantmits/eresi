/*
**
** $Id: asm_sparc_impdep1.c,v 1.3 2007-03-07 16:45:34 thor Exp $
**
*/
#include "libasm.h"

int
asm_sparc_impdep1(asm_instr * ins, u_char * buf, u_int len,
		  asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);
  
  ins->type = ASM_TYPE_ARCH;

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];

  return 4;
}