/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

/* TLTU rs,rt */

int asm_mips_tltu(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_TLTU;
   asm_mips_operand_t(&ins->op[0], buf, ASM_MIPS_OTYPE_TRAP, ins);

   return 777;
}