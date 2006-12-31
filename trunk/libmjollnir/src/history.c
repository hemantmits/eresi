/*
** (C) 2001-2006 Devhell Labs / Asgard Labs : thorolf / sk / mayhem
**
** $Id: history.c,v 1.6 2006-12-31 21:38:08 thor Exp $
*/
#include "libmjollnir.h"

/* Oldest entry is entry 0 */
/* It is the good way to do it */
void	mjr_history_shift(mjrcontext_t *cur, asm_instr inst, elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (inst.instr == ASM_NOP)
    ELFSH_PROFILE_OUT(__FILE__,__FUNCTION__,__LINE__);

  memcpy(&cur->hist[0], &cur->hist[1], sizeof(mjrhistory_t));
  memcpy(&cur->hist[1], &cur->hist[2], sizeof(mjrhistory_t));
  memcpy(&cur->hist[2], &cur->hist[3], sizeof(mjrhistory_t));
  memcpy(&cur->hist[3], &cur->hist[4], sizeof(mjrhistory_t));
  memcpy(&cur->hist[4].instr, &inst, sizeof(asm_instr));
  cur->hist[4].vaddr = addr;

  ELFSH_PROFILE_OUT(__FILE__,__FUNCTION__,__LINE__);
}

/* Write an entry of history */
void	mjr_history_write(mjrcontext_t *cur, asm_instr *i, elfsh_Addr a, int idx)
{
  if (idx >= MJR_HISTORY_LEN)
    return;

  memcpy(&cur->hist[idx].instr, i, sizeof(asm_instr));
  cur->hist[idx].vaddr = a;
}

