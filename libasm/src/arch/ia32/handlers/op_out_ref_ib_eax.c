/*
** $Id: op_out_ref_ib_eax.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_out_ref_ib_eax" opcode="0xe7"/>
*/

int op_out_ref_ib_eax(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->ptr_instr = opcode;
    new->instr = ASM_OUT;
    new->op1.type = ASM_OTYPE_IMMEDIATE;
    new->op1.content = ASM_OP_VALUE;
    new->op1.len = 1;
    new->op1.imm = 0;
    memcpy(&new->op1.imm, opcode + 1, 1);

    new->op2.type = ASM_OTYPE_FIXED;
    new->op2.content = ASM_OP_BASE;
    new->op2.regset = ASM_REGSET_R32;
    new->op2.base_reg = ASM_REG_EAX;
    new->len += new->op1.len;

    return (new->len);
}