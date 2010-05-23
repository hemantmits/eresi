/**
* @file libasm/src/arch/arm/handlers/asm_arm_clz.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_clz(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  struct s_arm_decode_dataproc opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_dataproc(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->dataproc_table[opcode.cond];

  ins->name = ins->proc->instr_table[ins->instr];

  MASSIGNTYPE(ins, ASM_TYPE_BITTEST);

  ins->nb_op = 2;

  /* Decode operands */
  ins->op[0].baser = opcode.rd;
  ins->op[0].destination = 1;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  ins->op[1].baser = opcode.rm;
  asm_arm_op_fetch(&ins->op[1], buf, ASM_ARM_OTYPE_REGISTER, ins);

  if (MISTYPE(ins, ASM_TYPE_BRANCH)
      || MISTYPE(ins, ASM_TYPE_CALLPROC)
      || MISTYPE(ins, ASM_TYPE_RETPROC))
    {
      MASSIGNTYPE(ins, ASM_TYPE_INDCONTROL);
    }

  LIBASM_PROFILE_FOUT(4);
}
