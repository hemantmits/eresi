/*
** hijack.c in elfsh
** 
** Started on  Tue Feb  4 14:41:34 2003 emsi
**
*/
#include "libelfsh.h"


/* Perform custom function hijack, return 0 on success -1 on error */
int		elfsh_hijack_function_by_name(elfshobj_t *file, 
					      uint32_t	 type,
					      char	 *name, 
					      elfsh_Addr addr)
{
  u_char	archtype;
  u_char	objtype;
  u_char	ostype;
  elfsh_Sym	*symbol;
  int		ret;
  int		ispltent;
  elfshsect_t	*hooks;
  uint32_t	pgsize;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  elfsh_setup_hooks();

  /* Sanity checks */
  if (file == NULL || name == NULL || addr == 0) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  if (elfsh_copy_plt(file, elfsh_get_pagesize(file)) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to copy PLT", -1);

  archtype = elfsh_get_archtype(file);
  objtype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      objtype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid target", -1);

  /* Guess the hijack type */
  switch (type) 
    {
      
      /* static function hijacking */
      /* This type is general enough for redirecting on the good hijack technique
	 if it finds that the current file architecture is not suited to the 
	 requested hijack type.
      */
    case ELFSH_HIJACK_TYPE_FLOW:

      /* If the hook section does not exist, create it */
      hooks = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_HOOKS, 0, 0, 0); 
      if (!hooks)
	{
	  pgsize = elfsh_get_pagesize(file);
	  hooks = elfsh_insert_section(file, 
				       ELFSH_SECTION_NAME_HOOKS, 
				       NULL,
				       ELFSH_CODE_INJECTION, 
				       pgsize - 1, pgsize);
	  if (!hooks)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Cannot get and inject .hooks", -1);
	  hooks->curend = 0;
	}

      symbol = elfsh_get_symbol_by_name(file, name);

      ispltent = elfsh_is_pltentry(file, symbol);
#if __DEBUG_REDIR__
      printf("[DEBUG_REDIR] 1 ispltent = %u for symbol %s resolved at %08X \n", 
	     ispltent, name, symbol ? symbol->st_value : 0);
#endif
      if (NULL == symbol || ispltent == 1)
	{
	  symbol = elfsh_get_dynsymbol_by_name(file, name);
	  if (!symbol)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unknown function (no symbol)", -1);
	  ispltent = elfsh_is_pltentry(file, symbol);
	  
#if __DEBUG_REDIR__
	  printf("[DEBUG_REDIR] 2 ispltent = %u for symbol %s resolved at %08X \n",
		 ispltent, name, symbol ? symbol->st_value : 0);
#endif

	  /* Use elfsh 0.6 hooks, redirect on a PLT hijack if necessary */
	  if (ispltent <= 0)
	    {
	      ret = (*hook_cflow[archtype][objtype][ostype])(file, name, symbol, addr);
	      if (ret < 0)
		ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
				  "Unable to perform CFLOW", -1);
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,  0);
	    }

	  /* Temporary .. on MIPS */
	  if (FILE_IS_MIPS(file))
	    {
	      ret = elfsh_set_got_entry_by_name(file, name, addr);
	      if (ret < 0) 
		ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
				  "Unable to patch GOT entry", -1);
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	    }
	  
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     ((*hook_plt[archtype][objtype][ostype])(file, symbol, addr)));
	}

      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (*hook_cflow[archtype][objtype][ostype])(file, name, symbol, addr));

      /* GOT entry hijacking */
    case ELFSH_HIJACK_TYPE_GOT:
      ret = elfsh_set_got_entry_by_name(file, name, addr);
      if (ret < 0) 
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to patch GOT entry", -1);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      
      /* PLT hijack */
    case ELFSH_HIJACK_TYPE_PLT:
      if (FILE_IS_MIPS(file))
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "There is not PLT to hijack on MIPS", -1);

      symbol = elfsh_get_dynsymbol_by_name(file, name);
      if (NULL == symbol)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown dynamic symbol", -1);

      if (!elfsh_is_pltentry(file, symbol))
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Symbol is not a PLT entry", -1);
      
      /* Now use ELFsh 0.6 hooks model */
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 ((*hook_plt[archtype][objtype][ostype])(file, symbol, addr)));
    }
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unknown redirection type", -1);
}
