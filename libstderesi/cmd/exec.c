/**
** @file exec.c
** 
** Started on  Tue Feb 18 13:03:14 2003 jfv
**
** $Id: exec.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"

/* Fork and execve the stuff */
int		cmd_exec()
{
  int       status;
  char      buf[BUFSIZ] = "";
  int       i;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_terminal_unprepare(world.state.revm_mode);

  for (i = 0; world.curjob->curcmd->param[i]; i++)
    {
      strncat (buf, " ", BUFSIZ); 
      strncat (buf, world.curjob->curcmd->param[i], BUFSIZ);
    }
  
  switch (status = revm_system (buf))
    {
    case 0:
      revm_output("\n [*] Command executed successfully \n\n");
      break;
    default:
      revm_output("\n [E] Command failed\n\n");
    }
  
  revm_terminal_prepare(world.state.revm_mode);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     status);
}
