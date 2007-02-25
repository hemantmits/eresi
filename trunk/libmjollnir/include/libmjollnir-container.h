
/*
 * 2001-2007 Devhell Labs
 * 2007 Asgard Labs, thorolf
 * 
 * Container structures for libmjollnir
 *
 * $Id: libmjollnir-container.h,v 1.1 2007-02-25 16:03:46 thor Exp $
 *
 */

#if !defined(__MJR_CONTAINER__)
 #define __MJR_CONTAINER__ 1

#include "libelfsh.h"

typedef struct		s_container
{
  void			*data;   /* points to the desired object */
  u_int			type;    /* type of the object */
  mjrlink_t		*input;  /* a way we came to this container */
  mjrlink_t		*output; /* a way to the next container */
}			mjrcontainer_t;

#endif