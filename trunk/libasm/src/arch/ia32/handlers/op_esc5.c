/*
** $Id: op_esc5.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_esc5" opcode="0xdd"/>
*/

int op_esc5(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  new->ptr_instr = opcode;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  if (modrm->mod == 3)
    switch(modrm->r) {
    case 3:
      new->instr = ASM_FSTP;
      break;
    case 4:
      new->instr = ASM_FUCOM;
      break;
    case 5:
      new->instr = ASM_FUCOMP;
      break;
    default:
      new->instr = ASM_BAD;
      break;
    }
  else
    switch (modrm->r) {
    case 0:
      new->instr = ASM_FLD;
      break;
    case 1:
      new->instr = ASM_BAD;
      break;
    case 2:
      new->instr = ASM_FST;
      break;
    case 3:
      new->instr = ASM_FSTP;
      break;
    case 4:
      new->instr = ASM_BAD; // ASM_FRSTOR;
      break;
    case 5:
      new->instr = ASM_BAD; // ;
      break;
    case 6:
      new->instr = ASM_BAD;
      break;
    case 7:
      new->instr = ASM_BAD;
      break;
    }
  
  if (modrm->mod == 3) 
    {
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_FIXED, 
				    proc);
      new->op1.content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
      new->op1.len = 1;
      new->op1.ptr = opcode + 1; 
      new->op1.scale = modrm->m;
#else
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
      new->op1.len = 1;
      new->op1.ptr = opcode + 1; 
      new->op1.scale = modrm->m;
#endif
    } 
  else 
    {
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				    proc);
#else
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
#endif
    }
#if LIBASM_USE_OPERAND_VECTOR
#else
  if (new->op1.type)
    new->len += new->op1.len;
#endif
  return (new->len);
}