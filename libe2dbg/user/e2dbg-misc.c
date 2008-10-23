/**
 * @file e2dbg-misc.c
 *
 * Misc routines for the debugger
 * 
 * Started on  Fri Jun 05 15:21:56 2005 jfv
 *
 *
 * $Id: e2dbg-misc.c,v 1.1 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libe2dbg.h"


/** 
 * Get the parent object of a breakpoint.
 * Thats needed for the mprotect stuff inside the breakpoint handler 
 * @param addr
 * @return
 */
elfshobj_t      *e2dbg_get_parent_object(eresi_Addr addr)
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


/**
 * Realize the output.
 *  @todo: XXX: Need a vector
 * @param str
 * @return
 */
int		e2dbg_output(char *str)
{
  //revm_output(str);
  fprintf(stderr, str);
  return (0);
}


/**
 * Get the identity of the current process or thread 
 * @return 
 */
int		e2dbg_self()
{
#if 0
  printf("Entering e2dbg_self : threadnbr = %u \n", e2dbgworld.threadnbr);
#endif

  if (e2dbgworld.threadnbr == 1)
    return (getpid());
  return ((int) pthread_self());
}


/**
 * Send a signal 
 * @param pid
 * @param sig
 * @return
 */
int		e2dbg_kill(int pid, int sig)
{
  if (e2dbgworld.threadnbr == 1)
    return (kill(pid, sig));
  else
    return (pthread_kill((pthread_t) pid, sig));
}

/**
 * Determine stack address 
 * @param cur
 * @return
 */
static void	e2dbg_stack_get(e2dbgthread_t *cur)
{
  struct rlimit	rlp;
  int		index;

#if __DEBUG_THREADS__
  int		ret;
  char		logbuf[BUFSIZ];
#endif

  getrlimit(RLIMIT_STACK, &rlp);
  cur->stacksize = rlp.rlim_cur;

  cur->stackaddr = (eresi_Addr) environ;

  for (index = 0; environ[index]; index++)
    {
      if ((eresi_Addr) environ[index] > cur->stackaddr)
	cur->stackaddr = (eresi_Addr) environ[index];
      if ((eresi_Addr) (environ + index) > cur->stackaddr)
	cur->stackaddr = (eresi_Addr) environ + index;
    }

  cur->stackaddr = cur->stackaddr - cur->stacksize;

#if __DEBUG_THREADS__
  ret = snprintf(logbuf, BUFSIZ, 
		 "\n [D] Thread ID %u has stack at addr %08X with size %u (environ = %08X) max = %08X\n",
		 (unsigned int) cur->tid, cur->stackaddr, cur->stacksize, 
		 (eresi_Addr) environ, (eresi_Addr) (cur->stackaddr + cur->stacksize));
  write(1, logbuf, ret);
#endif

}


/**
 * Only called when running a monothread program 
 * @return
 */
int		e2dbg_curthread_init(void)
{
  e2dbgthread_t	*new;
  char		*key;

#if __DEBUG_THREADS__
  char		logbuf[BUFSIZ];
  int		ret;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(e2dbgthread_t), -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,key, 15, -1);
  snprintf(key, 15, "%u", (unsigned int) getpid());
  new->tid     = (pthread_t) getpid();
  new->entry   = (void *) e2dbgworld.real_main;
  new->initial = 1;
  time(&new->stime);
  hash_add(&e2dbgworld.threads, key, new);
  e2dbgworld.curthread = new;
  e2dbgworld.threadnbr = 1;
  e2dbg_stack_get(new);

#if __DEBUG_THREADS__
  ret = snprintf(logbuf, BUFSIZ, " [D] Thread INITIAL has ID %u, stack at addr %08X with size %u \n",
		 (unsigned int) new->tid, new->stackaddr, new->stacksize);
  write(1, logbuf, ret);
#endif


  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
