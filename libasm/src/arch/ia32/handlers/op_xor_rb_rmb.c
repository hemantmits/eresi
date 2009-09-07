/**
 * @file op_xor_rb_rmb.c
 * @ingroup libasm/ia32/handlers
 * @brief Handler for instruction xor rb,rmb opcode 0x32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction xor rb,rmb opcode 0x32
 * @param instr Pointer to instruction structure
 * @param  opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
*/

int op_xor_rb_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_XOR;
  new->len += 1;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERALBYTE, new, 0);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODEDBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERALBYTE, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODEDBYTE, new);
#endif  

  return (new->len);
}
