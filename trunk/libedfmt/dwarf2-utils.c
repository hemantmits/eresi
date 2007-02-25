/*
** dwarf2-utils.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 26 2006 10:49:45 mxatone
**
*/

#include "libedfmt.h"

/* Read an unsigned leb128 number */
u_long 			edfmt_read_uleb128(void *data, u_int *bread)
{
  u_long		sum = 0;
  u_int			read = 0;
  u_char 		c;
  int			s = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  do {
    c = *(u_char *) (data + read) & 0xFF; 
    read++;
    sum |= ((u_long)(c & 127) << s);
    
    s += 7;
  } while ((c & 128) != 0);

  if (bread)
    *bread = read;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sum);
}

/* Read a signed leb128 number */
long 			edfmt_read_leb128(void *data, u_int *bread)
{
  long			sum = 0;
  u_int			read = 0;
  u_char 		c;
  int			s = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  do {
    c = *(u_char *) (data + read) & 0xFF; 
    read++;
    sum |= ((long)(c & 127) << s);
    
    s += 7;
  } while ((c & 128) != 0);

  if ((s < 8 * sizeof(sum)) && (c & 0x40))
    sum |= -(((long)1) << s);

  if (bread)
    *bread = read;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sum);
}