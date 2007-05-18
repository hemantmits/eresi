/*
** $Id: op_add_rmb_rb.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 * Opcode :             0x00
 * ADD
 * Destination is an encoded byte.
 * Source is a byte register.
 */

int op_add_rmb_rb(asm_instr *new, u_char *opcode, u_int len,
                  asm_processor *proc) {
  new->instr = ASM_ADD;
  new->type = ASM_TYPE_ARITH;
  new->ptr_instr = opcode;
  new->len++;
  /* new->type = IS_MEM_READ */
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERALBYTE, proc);
  #else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmb_rb(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}