/**
 * @file i386_bt_rm_r.c
 * $Id: i386_bt_rm_r.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction bt rm, r, opcode 0x0f 0xa3
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int i386_bt_rm_r(asm_instr *new, u_char *opcode, u_int len,
                 asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->instr = ASM_BT;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, 
				proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}