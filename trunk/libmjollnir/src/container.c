/*
 * 2001-2007 Devhell Labs
 * 2007 Asgard Labs, thorolf
 * 
 * Container related API
 *
 * $Id: container.c,v 1.7 2007-04-02 18:01:19 may Exp $
 *
 */

#include "libmjollnir.h"

static mjrcontainer_t **reg_containers;
static unsigned int cntnrs_size = MJR_CNTNRS_INCREMENT;
static unsigned int next_id = 1;

/**
 *
 */
void mjr_init_containers()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, reg_containers, 
	 sizeof(mjrcontainer_t*) * cntnrs_size, );
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 *
 */
void mjr_resize_containers()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  cntnrs_size += MJR_CNTNRS_INCREMENT;
  XREALLOC(__FILE__, __FUNCTION__, __LINE__, reg_containers, reg_containers,
	   sizeof(mjrcontainer_t*) * cntnrs_size, );
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 *
 */
unsigned int mjr_register_container(mjrcontainer_t *cntnr)
{
  if (next_id >= cntnrs_size)
    {
      if (cntnrs_size < MJR_CNTNRS_INCREMENT * MJR_MAX_INCREMENTS)
	{
	  mjr_resize_containers();
	}
      else
	{
	  return 0;
	}
    }
  
  reg_containers[next_id] = cntnr;
  cntnr->id = next_id;

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: %x registred id: %d type: %d\n",
	  __FUNCTION__, 
	  (cntnr->type == 1) ? 
	  ((mjrblock_t *)cntnr->data)->vaddr :
	  ((mjrfunc_t *)cntnr->data)->vaddr, cntnr->id, cntnr->type);
#endif

  return next_id++;
}

/**
 *
 */
unsigned int mjr_register_container_id (mjrcontainer_t *cntnr)
{
  if (cntnr->id >= cntnrs_size)
    {
      if (cntnrs_size < MJR_CNTNRS_INCREMENT * MJR_MAX_INCREMENTS)
	{
	  mjr_resize_containers();
	}
      else
	{
	  return 0;
	}
    }
  
  reg_containers[cntnr->id] = cntnr;
  if (cntnr->id >= next_id)
    next_id = cntnr->id + 1;

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: %x registred id: %d\n",
	  __FUNCTION__, 
	  (cntnr->type == 1) ? 
	  ((mjrblock_t *)cntnr->data)->vaddr :
	  ((mjrfunc_t *)cntnr->data)->vaddr, cntnr->id);
#endif

  return cntnr->id;
}

/**
 *
 */
void mjr_unregister_container (unsigned int id)
{
	reg_containers[id] = NULL;
}

/**
 *
 */
mjrcontainer_t *mjr_lookup_container (unsigned int id)
{
	return reg_containers[id];
}

/**
 * This functions removes links marked as MJR_LINK_DELETE
 */
int mjr_container_link_cleanup(mjrcontainer_t *c,int direction)
{
  mjrlink_t *new,*cur,*tmp;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  new = NULL;

  /* get direction */
  cur = mjr_link_get_by_direction(c,direction);

  /* walk links and pickup active one */
  while(cur)
    {
      if (cur->type != MJR_LINK_DELETE)
	{
	  if (!new)
	    new = cur;
	  else
	    new->next = cur;
	}
      tmp = cur;
      cur = cur->next;

      /* remove old one */
      if (tmp->type == MJR_LINK_DELETE)
	{
	  XFREE(__FILE__,__FUNCTION__,__LINE__,tmp);
	  if (direction == MJR_LINK_IN)
	    c->in_nbr--;
	  else if (direction == MJR_LINK_OUT)
	    c->out_nbr--;
	}
    }

  /* update container */
  if (direction == MJR_LINK_IN)
    c->input = new;
  else if (direction == MJR_LINK_OUT)
    c->output = new;
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 *
 */
mjrlink_t *mjr_container_add_link (mjrcontainer_t *cntnr,
				   unsigned int id,
				   int link_type,
				   int link_direction)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  mjrlink_t *cur;
  mjrlink_t *link;

#if __DEBUG_CNTNR__
  mjrcontainer_t *cnt;
  u_int vaddr1,vaddr2;

  if (cntnr->id == id)
    fprintf(D_DESC,"[D] %s: linking the same container id:%d\n",
	    __FUNCTION__,id);
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__, link, 
	 sizeof(mjrlink_t), NULL );

  link->id = id;
  link->type = link_type;
  link->next = NULL;

#if __DEBUG_CNTNR__

  if (cntnr->type == MJR_CNTNR_FUNC)
    vaddr1 = ((mjrfunc_t *)cntnr->data)->vaddr;
  else if (cntnr->type == MJR_CNTNR_BLOCK)
    vaddr1 = ((mjrblock_t *)cntnr->data)->vaddr;

  cnt = mjr_lookup_container(id);
  if (cnt->type == MJR_CNTNR_FUNC)
    vaddr2 = ((mjrfunc_t *)cnt->data)->vaddr;
  else if (cntnr->type == MJR_CNTNR_BLOCK)
    vaddr2 = ((mjrblock_t *)cnt->data)->vaddr;

  fprintf(D_DESC,"[D] %s: linking id:%d<%x> LD:%d id:%d<%x> LT:%d\n",
	  __FUNCTION__,
	  cntnr->id,
	  vaddr1,
	  link_direction,
	  id,
	  vaddr2,
	  link_type);
#endif

  if (link_direction == MJR_LINK_IN) 
    {
      cur = cntnr->input;
      cntnr->in_nbr++;
      if (!cur)
	{
	  cntnr->input = link;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, link);
	}
    }
  else if (link_direction == MJR_LINK_OUT)
    {
      cur = cntnr->output;
      cntnr->out_nbr++;
      if (!cur)
	{
	  cntnr->output = link;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, link);
	}
    }
  
  while (cur->next)
    {
      assert(cur != cur->next);
      cur = cur->next;
    }

  cur->next = link;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, link);
}

/**
 *
 */
mjrlink_t *mjr_get_link_of_type(mjrlink_t *link, int link_type)
{
  if (!link || link->type == link_type)
    return link;
  else 
    return mjr_get_link_of_type(link->next, link_type);
}

/**
 *
 */
mjrcontainer_t *mjr_create_block_container(u_int symoff,
					   elfsh_Addr vaddr,
					   u_int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mjrblock_t 		*newblock;
  mjrcontainer_t	*newcntnr;

#if __DEBUG_CNTNR__
  newcntnr = mjr_get_container_by_vaddr(vaddr,MJR_CNTNR_BLOCK);
  if (newcntnr)
    {
      fprintf(D_DESC,"[D] %s: block container %x id:%d is there\n",
	      __FUNCTION__, vaddr, newcntnr->id);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
    }
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__, newblock, sizeof(mjrblock_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newcntnr, sizeof(mjrcontainer_t), NULL);
  
  newblock->symoff = symoff;
  newblock->vaddr = vaddr;
  newblock->size = size;
  
  newcntnr->data = newblock;
  newcntnr->type = MJR_CNTNR_BLOCK;
  newcntnr->input = NULL;
  newcntnr->in_nbr = 0;
  newcntnr->output = NULL;
  newcntnr->out_nbr = 0;

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: create block %x (%d)\n",
	  __FUNCTION__, vaddr, size);
#endif
  mjr_register_container(newcntnr);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}

/**
 * Creates function container
 */
mjrcontainer_t *mjr_create_function_container(elfsh_Addr vaddr,
					      u_int size,
					      char *name,
					      mjrblock_t *first,
					      char *md5)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mjrfunc_t		*newfunction;
  mjrcontainer_t	*newcntnr;
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newfunction, sizeof(mjrfunc_t), NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newcntnr, sizeof(mjrcontainer_t), NULL);
  
  newfunction->vaddr = vaddr;
  newfunction->size = size;
  if (name)
    strncpy(newfunction->name, name, sizeof(newfunction->name)-1);
  else
    newfunction->name[0] = 0;
  
  newfunction->first = NULL;
  if (md5)
    memcpy(newfunction->md5, md5, sizeof(newfunction->md5));
  else
    memset(newfunction->md5, 0, sizeof(newfunction->md5));
  
  newcntnr->data = newfunction;
  newcntnr->type = MJR_CNTNR_FUNC;
  newcntnr->input = NULL;
  newcntnr->in_nbr = 0;
  newcntnr->output = NULL;
  newcntnr->out_nbr = 0;

#if __DEBUG_CNTNR__
  fprintf(D_DESC,"[D] %s: create func %x/<%s> (%d) %s\n",
	  __FUNCTION__, vaddr, name, size, md5);
#endif

  mjr_register_container(newcntnr);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}

/**
 * This function should be used only for debug
 * it O(n) since this api doesn't know about hashes in the mjr context
 */

mjrcontainer_t *mjr_get_container_by_vaddr(elfsh_Addr vaddr,int type)
{
  mjrcontainer_t	*cur;
  u_int			idx;

  PROFILER_IN(__FILE__,__FUNCTION__,__LINE__);

  idx=1;
  while((cur=reg_containers[idx++]))
    {
      if ((cur->type == type) && (cur->type == MJR_CNTNR_FUNC))
	{
	  if (((mjrfunc_t *)cur->data)->vaddr == vaddr)
	    PROFILER_ROUT(__FILE__,__FUNCTION__,__LINE__,(mjrcontainer_t *)cur);
	}    
      else if ((cur->type == type ) && (cur->type == MJR_CNTNR_BLOCK))
	{
	  if (((mjrblock_t *)cur->data)->vaddr == vaddr)
	    PROFILER_ROUT(__FILE__,__FUNCTION__,__LINE__,(mjrcontainer_t *)cur);
	}
    }
  PROFILER_ROUT(__FILE__,__FUNCTION__,__LINE__,(mjrcontainer_t *)NULL);    
}

void mjr_container_dump(int what)
{
  mjrcontainer_t	*cur;
  mjrfunc_t		*tf;
  u_int			idx;
  idx=1;
  while((cur=reg_containers[idx++]))
    {
      if (cur->type == what)
	{
	  if (cur->type == MJR_CNTNR_FUNC)
	    {
	      tf = cur->data;
	      printf("FOUND: FUNC T:%d V:%x I:%d O:%d\n",
		     what,
		     tf->vaddr,
		     cur->in_nbr,
		     cur->out_nbr);
	    }
	}
    }
}

mjrlink_t	*mjr_link_get_by_direction(mjrcontainer_t *c,int dir)
{
  mjrlink_t *ret;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = NULL;

  if (dir == MJR_LINK_IN)
    ret = c->input;
  else if (dir == MJR_LINK_OUT)
    ret = c->output;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (mjrlink_t *)ret);
}

