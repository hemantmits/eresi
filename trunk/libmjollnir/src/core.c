/*
** (C) 2006 Devhell Labs / Asgard Labs : sk, mayhem, thorolf
** 
** Implement low-level functions of the libmjollnir library
**
** $Id: core.c,v 1.13 2006-12-29 22:38:47 may Exp $
*/
#include "libmjollnir.h"


/* This function will find calls including calls trought a pointer */
int		  mjr_analyse_section(mjrsession_t *sess, char *section_name) 
{
  
  elfshsect_t    *sct;
  asm_instr       instr;
  unsigned char  *ptr;
  unsigned long   curr, len;
  u_int           vaddr, ilen;
  elfsh_Addr      e_point;
  elfsh_Addr	  main_addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* load section */
#if __DEBUG_MJOLLNIR__
  fprintf(D_DESC, "[__DEBUG_MJOLLNIR__] mjrFindCalls: loading %s\n",section_name);
#endif
  
  sct = elfsh_get_section_by_name(sess->cur->obj, section_name, NULL, NULL, NULL);
  if (!sct)
    ELFSH_NOPROFILE_ROUT(NULL);
  
#if __DEBUG_CALLS__
  fprintf(D_DESC, "[__DEBUG_CALLS__] mjrFindCalls: %x %d %d\n",
	  sct->shdr->sh_addr, sct->shdr->sh_size, sct->shdr->sh_offset);
#endif
  
  /* Setup initial conditions */
  ptr      = elfsh_get_raw(sct);
  len      = sct->shdr->sh_size;
  curr     = 0;
  vaddr    = sct->shdr->sh_addr;
  e_point  = elfsh_get_entrypoint(elfsh_get_hdr(sess->cur->obj));
  if (sct->shdr->sh_addr == e_point)
    {
      printf(" [*] Entry point: %08x\n", e_point);
      main_addr = mjr_trace_start(sess->cur, sess->cur->obj, ptr, 
				  sct->shdr->sh_size, e_point, 
				  &sess->cur->blklist);
      printf(" [*] main located at %08x\n", main_addr);
    }

  /* Read all instructions of the section */
  while (curr < len) 
    {
      ilen = asm_read_instr(&instr, ptr + curr, len - curr, &sess->cur->proc);
      if (ilen > 0) 
	{
	  mjr_history_shift(sess->cur, instr, vaddr + curr);
	  mjr_trace_control(sess->cur, sess->cur->obj, &instr, vaddr + curr);
	} 
    }
  
  /* Store analyzed blocks in file */
  if (mjr_store_blocks(sess->cur->obj, sess->cur->blklist, 1) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to store blocks in file", -1);    
  
  elfsh_free(ptr);
  ELFSH_NOPROFILE_ROUT(NULL);
}



/* Main analysis function */
int		mjr_analyse(mjrsession_t *sess, int flags) 
{
  char		*shtName;
  elfsh_Shdr	*shtlist, *shdr;
  elfsh_Sym	*sym;
  int		num_sht, idx_sht;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Preliminary checks */
  if ((NULL == sess) || (NULL == sess->cur))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameters", -1);
  
  /* Get section table */
  shtlist = elfsh_get_sht(sess->cur->obj, &num_sht);
  if (!shtlist)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to get SHT", -1);
  
  sess->cur->blklist = mjr_get_blocks(sess->cur->obj);

#if __DEBUG_MJOLLNIR__
  fprintf(D_DESC,"[__DEBUG__] mjr_analize: Found %d sections.\n",num_sht);
#endif
  
  /* Analyse all executable sections */
  for (idx_sht = 0; idx_sht < num_sht; idx_sht++) 
    {
      shdr    = (shtlist + idx_sht);
      sym     = elfsh_get_sym_from_shtentry(sess->cur->obj, shdr);
      shtName = elfsh_get_symbol_name(sess->cur->obj, sym);
      
      if (!elfsh_get_section_execflag(shdr) ||
	  !elfsh_get_section_allocflag(shdr))
	continue;
      
#if __DEBUG_MJOLLNIR__
      fprintf(D_DESC, 
	      "[__DEBUG__] mjr_analize: Executable section name=(%14s) "
	      "index=(%02i)\n", shtName, idx_sht);
#endif
      mjr_analyse_section(sess, shtName);
    }
  
  /* Set the flag and return */
  sess->cur->analysed = 1;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



//**** FIXME : should go in libasm******


/* Debug printing */
int	asm_debug_operand(asm_operand * op)
{
  fprintf(D_DESC, "[*] Operand Dump\n[*] len: %d type: %d size: %d content: %d\n",
	  op->len, op->type, op->size, op->content);
  
  fprintf(D_DESC, "[*] Content: %s %s %s %s %s %s %s\n",
	  (op->content & ASM_OP_VALUE) ? "ASM_OP_VALUE" : ".",
	  (op->content & ASM_OP_BASE) ? "ASM_OP_BASE" : ".",
	  (op->content & ASM_OP_INDEX) ? "ASM_OP_INDEX" : ".",
	  (op->content & ASM_OP_SCALE) ? "ASM_OP_SCALE" : ".",
	  (op->content & ASM_OP_FIXED) ? "ASM_OP_FIXED" : ".",
	  (op->content & ASM_OP_REFERENCE) ? "ASM_OP_REFERENCE" : ".",
	  (op->content & ASM_OP_ADDRESS) ? "ASM_OP_ADDRESS" : ".");
  
  return 0;
}

