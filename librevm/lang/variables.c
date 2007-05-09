/*
** variables.c for elfsh
**
** Contain functions for easy variables accesses
**
** Started September 16 2005 04:01:03 mayhem
**
**
** $Id: variables.c,v 1.4 2007-03-07 16:45:36 thor Exp $
**
*/
#include "revm.h"



/* Set a variable to a string value */
int		vm_setvar_str(char *varname, char *value)
{
  revmobj_t	*var;
  int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid NULL parameter", (-1));
  varname = vm_lookup_string(varname);
  var = hash_get(&vars_hash, varname);
  if (!var)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  if (vm_convert2str(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed string conversion", (-1));
  len = strlen(value) + 1;
  if (var->size < len)
    XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	     var->immed_val.str, var->immed_val.str, len, -1);
  memcpy(var->immed_val.str, value, len - 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Set a variable to a string value */
int             vm_setvar_raw(char *varname, char *value, u_int len)
{
  revmobj_t   *var;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid NULL parameter", (-1));
  varname = vm_lookup_string(varname);
  var = hash_get(&vars_hash, varname);
  if (!var)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unknown variable", (-1));
  if (vm_convert2raw(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed string conversion", (-1));
  if (var->size < len)
    XREALLOC(__FILE__, __FUNCTION__, __LINE__,
	     var->immed_val.str, var->immed_val.str, len, -1);
  memcpy(var->immed_val.str, value, len - 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/* Set a variable to a string value */
int		vm_setvar_byte(char *varname, u_char byte)
{
  revmobj_t	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", (-1));  
  varname = vm_lookup_string(varname);
  var = hash_get(&vars_hash, varname);
  if (!var)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  
  if (vm_convert2byte(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed byte conversion", (-1));

  var->immed_val.byte = byte;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Set a variable to a string value */
int		vm_setvar_short(char *varname, u_short val)
{
  revmobj_t	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", (-1));  
  varname = vm_lookup_string(varname);
  var = hash_get(&vars_hash, varname);
  if (!var)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));
  if (vm_convert2short(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed short conversion", (-1));
  var->immed_val.half = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Set a variable to a string value */
int		vm_setvar_int(char *varname, u_int val)
{
  revmobj_t	*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", (-1));  
  varname = vm_lookup_string(varname);
  var = hash_get(&vars_hash, varname);
  if (!var)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown variable", (-1));  
  if (vm_convert2int(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed integer conversion", (-1));
  var->immed_val.word = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/* Set a variable to a string value */
int             vm_setvar_long(char *varname, u_long val)
{
  revmobj_t   *var;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!varname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", (-1));  
  varname = vm_lookup_string(varname);
  var = hash_get(&vars_hash, varname);
  if (!var)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unknown variable", (-1));  
  if (vm_convert2long(var) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Failed long conversion", (-1));
  var->immed_val.ent = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}