/*
** output_sparc.c for  in /hate/home/hate/code/libasm_current
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Fri Jun 10 14:06:47 2005 #!HATE#@!
** Last update Thu Jun 16 05:41:18 2005 #!HATE#@!
*/

#include <libasm.h>

void asm_resolve_sparc(void *d, u_int val, char *buf, u_int len)
{
  sprintf(buf, "0x%x", val);
}

char *get_sparc_register(int reg)
{
  switch(reg) {
    case ASM_REG_G0: return "%g0";
    case ASM_REG_G1: return "%g1";
    case ASM_REG_G2: return "%g2";
    case ASM_REG_G3: return "%g3";
    case ASM_REG_G4: return "%g4";
    case ASM_REG_G5: return "%g5";
    case ASM_REG_G6: return "%g6";
    case ASM_REG_G7: return "%g7";
    case ASM_REG_O0: return "%o0";
    case ASM_REG_O1: return "%o1";
    case ASM_REG_O2: return "%o2";
    case ASM_REG_O3: return "%o3";
    case ASM_REG_O4: return "%o4";
    case ASM_REG_O5: return "%o5";
    case ASM_REG_O6: return "%sp";
    case ASM_REG_O7: return "%o7";
    case ASM_REG_L0: return "%l0";
    case ASM_REG_L1: return "%l1";
    case ASM_REG_L2: return "%l2";
    case ASM_REG_L3: return "%l3";
    case ASM_REG_L4: return "%l4";
    case ASM_REG_L5: return "%l5";
    case ASM_REG_L6: return "%l6";
    case ASM_REG_L7: return "%l7";
    case ASM_REG_I0: return "%i0";
    case ASM_REG_I1: return "%i1";
    case ASM_REG_I2: return "%i2";
    case ASM_REG_I3: return "%i3";
    case ASM_REG_I4: return "%i4";
    case ASM_REG_I5: return "%i5";
    case ASM_REG_I6: return "%fp";
    case ASM_REG_I7: return "%i7";
    default: return "bad";
  }  
}

/* Get SPARC state register */
char *get_sparc_sregister(int reg)
{
  switch(reg) {
    case ASM_SREG_Y: return "%y";    
    case ASM_SREG_CCR: return "%ccr";
    case ASM_SREG_ASI: return "%asi";
    case ASM_SREG_TICK: return "%tick";
    case ASM_SREG_PC: return "%pc";
    case ASM_SREG_FPRS: return "%fprs";
    default: return "bad";
  }  
}

/* Get SPARC privileged register */
char *get_sparc_pregister(int reg)
{
  switch(reg) {
	case ASM_PREG_TPC: return "%tpc";
  	case ASM_PREG_TNPC: return "%tnpc";
  	case ASM_PREG_TSTATE: return "%tstate";
  	case ASM_PREG_TT: return "%tt";
  	case ASM_PREG_TICK: return "%tick";
  	case ASM_PREG_TBA: return "%tba";
  	case ASM_PREG_PSTATE: return "%pstate";
  	case ASM_PREG_TL: return "%tl";
  	case ASM_PREG_PIL: return "%pil";
  	case ASM_PREG_CWP: return "%cwp";
  	case ASM_PREG_CANSAVE: return "%cansave";
  	case ASM_PREG_CANRESTORE: return "%canrestore";
  	case ASM_PREG_CLEANWIN: return "%cleanwin";
  	case ASM_PREG_OTHERWIN: return "%otherwin";
  	case ASM_PREG_WSTATE: return "%wstate";
  	case ASM_PREG_FQ: return "%fq";
  	case ASM_PREG_VER: return "%ver";
  	default: return "bad";
  }
}

/* Get SPARC condition code */
char *get_sparc_cc(int cc)
{
  switch(cc) {
  	case ASM_SP_FCC0: return "%fcc0";
  	case ASM_SP_FCC1: return "%fcc1";
  	case ASM_SP_FCC2: return "%fcc2";
  	case ASM_SP_FCC3: return "%fcc3";
  	case ASM_SP_ICC: return "%icc";
  	case ASM_SP_XCC: return "%xcc";
  	default: return "bad";
  }
}

void	asm_sparc_dump_operand(asm_instr *ins, int num, 
			       unsigned int addr, char *buf)
{
  asm_operand *op;
  int address;
  
  switch(num) {
    case 1: op = &ins->op1; break;
    case 2: op = &ins->op2; break;
    case 3: op = &ins->op3; break;
    default: return;
  }
  
  switch(op->type) {
    case ASM_SP_OTYPE_REGISTER:
      sprintf(buf, "%s", get_sparc_register(op->base_reg));
      break;
    case ASM_SP_OTYPE_SREGISTER:
      sprintf(buf, "%s", get_sparc_sregister(op->base_reg));
      break;
    case ASM_SP_OTYPE_PREGISTER:
      sprintf(buf, "%s", get_sparc_pregister(op->base_reg));
      break;
    case ASM_SP_OTYPE_FREGISTER:
      if (op->base_reg == ASM_FREG_FSR)
        sprintf(buf, "%%fsr");
      else
        sprintf(buf, "%%f%i", op->base_reg);
      break;
    case ASM_SP_OTYPE_CC:
      sprintf(buf, "%s", get_sparc_cc(op->base_reg));
      break;  
    case ASM_SP_OTYPE_IMMEDIATE:
      if (op->imm < 10)
		sprintf(buf, "%i", op->imm);
      else
		sprintf(buf, "0x%x", op->imm);
      break;
    case ASM_SP_OTYPE_SETHI:
      if (op->imm)
		sprintf(buf, "%%hi(0x%x)", op->imm << 10);
      else
		sprintf(buf, "%%hi(%x)", op->imm << 10);
      break;
    case ASM_SP_OTYPE_DISPLACEMENT:
      address = addr + (op->imm << 2);
      ins->proc->resolve_immediate(ins->proc->resolve_data, address, buf, 42);
      break;
    case ASM_SP_OTYPE_DISP30:
      address = addr + (op->imm << 2);
      ins->proc->resolve_immediate(ins->proc->resolve_data, address, buf, 42);
      break;
    case ASM_SP_OTYPE_IMM_ADDRESS:
      sprintf(buf, "[ %s", get_sparc_register(op->base_reg));
      if (op->imm)
        sprintf(buf, " + 0x%x", op->imm);
        
      sprintf(buf, " ]");
      
      if (op->address_space != 80)
        sprintf(buf, " %%asi");
      break;
    case ASM_SP_OTYPE_REG_ADDRESS:
      if (op->index_reg > 0)
        sprintf(buf, "[ %s + %s ]", get_sparc_register(op->base_reg),
      								get_sparc_register(op->index_reg));
      								
	  else
	    sprintf(buf, "[ %s ]", get_sparc_register(op->base_reg));
      								
      if (op->address_space != 80)
        sprintf(buf, " 0x%x", op->address_space);								
      break;
      
    default:
      sprintf(buf, "err");
  }
}

char *asm_sparc_display_instr(asm_instr *instr, int addr)
{
  static char buffer[1024];  
  memset(buffer, 0, 1024);
  sprintf(buffer, "%s ", instr->proc->instr_table[instr->instr]);  
  
  switch (instr->type) {
	case ASM_TYPE_CONDBRANCH:	  
	  if (instr->annul)
	    strcat(buffer, ",a");
	  if (!instr->prediction)
	    strcat(buffer, ",pn");
  
    default:
      if (instr->nb_op > 0) {
	    strcat(buffer, " ");
	    
	  	if (instr->nb_op == 3) {
	      asm_sparc_dump_operand(instr, 3, addr, buffer + strlen(buffer));
	      strcat(buffer, ", ");
	    }
	    if (instr->nb_op >= 2) {
	      asm_sparc_dump_operand(instr, 2, addr, buffer + strlen(buffer));
	      strcat(buffer, ", ");	    
	    }
	    asm_sparc_dump_operand(instr, 1, addr, buffer + strlen(buffer));
	  }    
  }
    
  return (buffer);    
}
