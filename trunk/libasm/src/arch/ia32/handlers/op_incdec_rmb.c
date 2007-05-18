/*
** $Id: op_incdec_rmb.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_incdec_rmb" opcode="0xfe"/>
*/

int op_incdec_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  struct s_modrm        *modrm;
  
  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_INC;
    break;
  case 1:
    new->instr = ASM_DEC;
    break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    break;
  default:
    break;
  }

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmb(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
#endif
  return (new->len);
}