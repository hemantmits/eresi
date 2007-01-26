/*
** $Id: op_int_ib.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_int_ib" opcode="0xcd"/>
*/

int op_int_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->instr = ASM_INT;
    new->len += 2;

    new->op1.type = ASM_OTYPE_IMMEDIATE;

    new->op1.content = ASM_OP_VALUE;
    new->op1.len = 1;
    new->op1.ptr = opcode + 1;

    if (*(opcode + 1) > 0x80u)
      memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
    else
      new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 1);
  return (new->len);
}
