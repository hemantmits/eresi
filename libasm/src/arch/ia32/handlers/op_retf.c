/*
** $Id: op_retf.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_retf" opcode="0xcb"/>
*/

int op_retf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_RETF;
   new->type = ASM_TYPE_RETPROC;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}