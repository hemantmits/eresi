/*
** $Id: op_loope.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_loope" opcode="0xe1"/>
 */

int op_loope(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 2;
    new->ptr_instr = opcode;
    new->instr = ASM_LOOPE;
    new->type = ASM_TYPE_CONDBRANCH;
    new->op1.type = ASM_OTYPE_JUMP;
    new->op1.size = ASM_OSIZE_BYTE;
    new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
    new->op1.ptr = opcode + 1;

    new->op1.imm = 0;
    if (*(opcode + 1) >= 0x80u)
      memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
    memcpy((char *) &new->op1.imm, opcode + 1, 1);
  return (new->len);
}
