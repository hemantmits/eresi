/*
** e2dbg-misc.c for e2dbg
**
** Misc routines for the debugger
** 
** Started on  Fri Jun 05 15:21:56 2005 mayhem
**
*/
#include "libe2dbg.h"


/* Get the parent object of a breakpoint */
/* Thats needed for the mprotect stuff inside the breakpoint handler */
elfshobj_t      *e2dbg_get_parent_object(elfsh_Addr addr)
{
  elfsh_Phdr    *cur;
  elfshobj_t    *curfile;
  elfshsect_t   *cursect;
  char          **keys;
  int           index;
  int           keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  keys = hash_get_keys(&world.curjob->loaded, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      curfile = hash_get(&world.curjob->loaded, keys[index]);
      cursect = elfsh_get_parent_section(curfile, addr, NULL);
      if (cursect)
        {
          cur = elfsh_get_parent_segment(curfile, cursect);
          if (cur)
            PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, curfile);
        }
    }
  
  /* Parent object not found */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unable to get parent object addr", (NULL));
}


/* Concatenate all parameters and return a single string */
char	*e2dbg_get_string(char **params)
{
  char	buff[BUFSIZ];
  int	idx;
  int	len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (len = idx = 0; params[idx]; idx++)
    len += snprintf(buff + len, BUFSIZ - len, "%s%s", 
		    (idx ? " " : ""), params[idx]);
   

  if (len)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       strdup(buff));
 
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Empty display", NULL);
}



/* Realize the output */
/* XXX: Need a vector */
int		e2dbg_output(char *str)
{
  vm_output(str);
  return (0);
}


/* Get the identity of the current process or thread */
int		e2dbg_self()
{
  if (e2dbgworld.threadnbr == 1)
    return (getpid());
  return (pthread_self());
}


/* Send a signal */
void		e2dbg_kill(pid_t pid, int sig)
{
  if (e2dbgworld.threadnbr == 1)
    kill(pid, sig);
  pthread_kill(pid, sig);
}
