/*
** api.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Jan 26 2007 11:54:22 mxatone
**
*/

#include "libedfmt.h"

/* Unique debug format base variable */
edfmtinfo_t *uniinfo = NULL;

u_char file_active = 0;
elfshobj_t *cu_obj = NULL;

#define API_GETPTR(_size) \
edfmt_alloc_pool(&(uniinfo->alloc_pool), &(uniinfo->alloc_pos), \
		 &(uniinfo->alloc_size), API_ALLOC_STEP, _size) 

#define EDFMT_NEW_TYPE(_type, _name, _up) 		       	\
do { 								\
  _type = edfmt_check_type(_name);				\
  if (_type)							\
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, _type);	\
  _type = API_GETPTR(sizeof(edfmttype_t)); 			\
  EDFMT_COPY_NAME(_type, _name);				\
  if (_up)							\
    edfmt_update_type(_type);					\
} while(0)

#define EDFMT_NEW_VAR(_var, _name, _up) 		       	\
do { 								\
  _var = edfmt_check_var(_name);				\
  if (_var)							\
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, _var);	\
  _var = API_GETPTR(sizeof(edfmtvar_t)); 			\
  EDFMT_COPY_NAME(_var, _name);					\
  if (_up)							\
    edfmt_update_var(_var);					\
} while(0)

#define EDFMT_NEW_FUNC(_func, _name, _up) 		       	\
do { 								\
  _func = edfmt_check_func(_name);				\
  if (_func)							\
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, _func);	\
  _func = API_GETPTR(sizeof(edfmtfunc_t)); 			\
  EDFMT_COPY_NAME(_func, _name);				\
  if (_up)							\
    edfmt_update_func(_func);					\
} while(0)

#define EDFMT_NEW_ARG(_arg, _name) 				\
do { 								\
  _arg = API_GETPTR(sizeof(edfmtfuncarg_t)); 			\
  EDFMT_COPY_NAME(_arg, _name);					\
} while(0)

/*
#define EDFMT_NEW_VAR(_var) \
do { _var = API_GETPTR(sizeof(edfmtvar_t)); } while(0)

#define EDFMT_NEW_FUNC(_func) \
do { _func = API_GETPTR(sizeof(edfmtfunc_t)); } while(0)
*/

#define EDFMT_COPY_NAME(_dest, _source) 			\
do { 								\
  int index, len;					       	\
  if (strlen(_source) >= EDFMT_NAME_SIZE)			\
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 		\
		       "Invalid name size", NULL);		\
  strcpy(_dest->name, _source);					\
  len = strlen(_dest->name);					\
  for (index = 0; index < len; index++)				\
    if (_dest->name[index] == ' ')				\
      _dest->name[index] = '_';					\
} while (0)

/* Init an add context */
int			edfmt_add_init(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  cu_obj = file;

  if (!file)							
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid file object", NULL);

  /* Init the object if needed */
  if (file->debug_format.uni == NULL) 
    {				
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     file->debug_format.uni, sizeof(edfmtinfo_t), NULL);	
      uniinfo = file->debug_format.uni;

      hash_init(&(uniinfo->htype), NULL, 30, ASPECT_TYPE_UNKNOW);
      hash_init(&(uniinfo->hvar), NULL, 30, ASPECT_TYPE_UNKNOW);
      hash_init(&(uniinfo->hfunc), NULL, 30, ASPECT_TYPE_UNKNOW);
    } 
  else 
    {							
      uniinfo = file->debug_format.uni; 
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* End an add context */
int			edfmt_add_end()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  while (uniinfo && uniinfo->lfile && uniinfo->lfile->parent != NULL)
    uniinfo->lfile = uniinfo->lfile->parent;

  /* Reset global pointers */
  cu_obj = NULL;
  uniinfo = NULL;
  file_active = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

#define API_GET_FROM_SCOPE(_name) \
uniinfo->lfile == NULL ? &(uniinfo->_name) : &(uniinfo->lfile->_name);

edfmttype_t	      	*edfmt_check_type(char *name)
{
  edfmttype_t		*seek_type = NULL;
  hash_t		*htable;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Global pointer uninitialise", NULL);

  /* Setup hash table depending of current scope */
  htable = API_GET_FROM_SCOPE(htype);

  /* Research on the right table */
  seek_type = (edfmttype_t *) hash_get(htable, name);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, seek_type);
}

edfmtvar_t	      	*edfmt_check_var(char *name)
{
  edfmtvar_t		*seek_var = NULL;
  hash_t		*htable;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Global pointer uninitialise", NULL);

  /* Setup hash table depending of current scope */
  htable = API_GET_FROM_SCOPE(hvar);

  /* Research on the right table */
  seek_var = (edfmtvar_t *) hash_get(htable, name);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, seek_var);
}

edfmtfunc_t	      	*edfmt_check_func(char *name)
{
  edfmtfunc_t		*seek_func = NULL;
  hash_t		*htable;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Global pointer uninitialise", NULL);

  /* Setup hash table depending of current scope */
  htable = API_GET_FROM_SCOPE(hfunc);

  /* Research on the right table */
  seek_func = (edfmtfunc_t *) hash_get(htable, name);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, seek_func);
}

/* Update type list */
int			edfmt_update_type(edfmttype_t *type)
{
  hash_t		*htable;
    
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Global pointer uninitialise", 0);

  /* Setup hash table depending of current scope */
  htable = API_GET_FROM_SCOPE(htype);

  type->file = uniinfo->lfile;

  /* Global scope */
  if (uniinfo->lfile == NULL)
    {
      if (uniinfo->types == NULL)
	uniinfo->types = type;
      else
	uniinfo->ltype->next = type;
      
      uniinfo->ltype = type;
    }
  else /* Local scope */
    {
      if (uniinfo->lfile->types == NULL)
	uniinfo->lfile->types = type;
      else
	uniinfo->lfile->ltype->next = type;
      
      uniinfo->lfile->ltype = type;
    }

  hash_add(htable, strdup(type->name), (void *) type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Update var list */
int			edfmt_update_var(edfmtvar_t *var)
{
  hash_t		*htable;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Global pointer uninitialise", NULL);

  /* Setup hash table depending of current scope */
  htable = API_GET_FROM_SCOPE(hvar);

  /* Global scope */
  if (uniinfo->lfile == NULL || file_active == 0)
    {
      if (uniinfo->vars == NULL)
	uniinfo->vars = var;
      else
	uniinfo->lvar->next = var;

      uniinfo->lvar = var;
    }
  else
    {
      /* Local scope */
      if (uniinfo->lfile->vars == NULL)
	uniinfo->lfile->vars = var;
      else
	uniinfo->lfile->lvar->next = var;

      uniinfo->lfile->lvar = var;
    }

  hash_add(htable, strdup(var->name), (void *) var);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Update function list */
int			edfmt_update_func(edfmtfunc_t *func)
{
  hash_t		*htable;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Global pointer uninitialise", NULL);

  /* Setup hash table depending of current scope */
  htable = API_GET_FROM_SCOPE(hfunc);

  /* Global scope */
  if (uniinfo->lfile == NULL || file_active == 0)
    {
      if (uniinfo->funcs == NULL)
	uniinfo->funcs = func;
      else
	uniinfo->lfunc->next = func;

      uniinfo->lfunc = func;
    }
  else
    {
      /* Local scope */
      if (uniinfo->lfile->funcs == NULL)
	uniinfo->lfile->funcs = func;
      else
	uniinfo->lfile->lfunc->next = func;

      uniinfo->lfile->lfunc = func;
    }

  hash_add(htable, strdup(func->name), (void *) func);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Retrieve uniinfo pointer */
edfmtinfo_t		*edfmt_get_uniinfo(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramter", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (edfmtinfo_t *) file->debug_format.uni);
}

int			edfmt_change_type_nfile(edfmttype_t *type)
{
  edfmttype_t		*tmp_type = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramter", -1);
  
  /* Delete for its file */
  if (type->file == NULL)
    {
      if (uniinfo->types == type)
	uniinfo->types = type->next;
      else
	tmp_type = uniinfo->types;
    }
  else
    {
      if (type->file->types == type)
	type->file->types = type->next;
      else
	tmp_type = type->file->types;
    }

  if (tmp_type != NULL)
    {
      /* Find our type in the file */
      while (tmp_type && tmp_type->next != type)
	tmp_type = tmp_type->next;
      
      /* We update the precedent file */
      if (tmp_type && tmp_type->next)
	tmp_type->next = type->next;
    }

  if (edfmt_check_type(type->name) == NULL)
    {
      /* Add to the current file */
      edfmt_update_type(type);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Restore parent file */
edfmtfile_t		*edfmt_restore_parent_file()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (uniinfo->lfile == NULL || uniinfo->lfile->parent == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No parents", NULL);

  uniinfo->lfile = uniinfo->lfile->parent;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, uniinfo->lfile);
}

edfmtfile_t		*edfmt_get_current_file()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No uniform informations", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, uniinfo->lfile);
}

/* Search for a specific included file */
edfmtfile_t		*edfmt_get_inc_file(edfmtfile_t *file, char *name)
{
  edfmtfile_t 		*inc;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (inc = file->child; inc != NULL; inc = inc->next)
    {
      if (!strcmp(name, inc->name))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, inc);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Add a new file */
edfmtfile_t		*edfmt_add_file(edfmtfile_t *parent_file, char *name, 
					elfsh_Addr start, elfsh_Addr end)
{
  edfmtfile_t 		*file, *already_file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramter", NULL);

  if (parent_file != NULL)
    {
      already_file = edfmt_get_inc_file(parent_file, name);

      if (already_file != NULL)
	{
	  uniinfo->lfile = already_file;
	  file_active = 1;	  

	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, already_file);
	}
    }

  XALLOC(__FILE__, __FUNCTION__, __LINE__,file, sizeof(edfmtfile_t), NULL);

  EDFMT_COPY_NAME(file, name);
  file->start = start;
  file->end = end;

  hash_init(&(file->htype), NULL, 30, ASPECT_TYPE_UNKNOW);
  hash_init(&(file->hvar), NULL, 30, ASPECT_TYPE_UNKNOW);
  hash_init(&(file->hfunc), NULL, 30, ASPECT_TYPE_UNKNOW);

  /* Update pointers depending if he get a parrent */
  if (parent_file != NULL)
    {
      if (parent_file->child == NULL)
	parent_file->child = file;
      else
	parent_file->lchild->next = file;

      parent_file->lchild = file;
      file->parent = parent_file;
    }
  else
    {
      if (uniinfo->files == NULL)
	uniinfo->files = file;
      else
	while (uniinfo->files->parent != NULL)
	  uniinfo->files = uniinfo->files->parent;
      
      if (uniinfo->lfile != NULL)
	uniinfo->lfile->next = file;
    }

  uniinfo->lfile = file;
  file_active = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, file);
}

/* Reset file activity */
int			edfmt_reset_file()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  file_active = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Reactive file activity */
int			edfmt_reactive_file()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  file_active = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create an unknown type */
edfmttype_t		*edfmt_add_type_unk(char *name)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 1);
  ltype->type = EDFMT_TYPE_UNK;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a basic type */
edfmttype_t		*edfmt_add_type_basic(char *name, int size)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 1);
  ltype->type = EDFMT_TYPE_BASIC;
  ltype->size = size;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create an array type */
edfmttype_t		*edfmt_add_type_array(char *name, 
					      int size, edfmttype_t *type)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 1);
  ltype->type = EDFMT_TYPE_ARRAY;
  ltype->parent = type;
  ltype->size = size;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a ptr type */
edfmttype_t		*edfmt_add_type_ptr(char *name, edfmttype_t *type)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 1);
  ltype->type = EDFMT_TYPE_PTR;
  ltype->parent = type;
  ltype->size = sizeof(void *);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a structure type */
edfmttype_t		*edfmt_add_type_struct(char *name, int size)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 1);
  ltype->type = EDFMT_TYPE_STRUCT;
  ltype->size = size;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create an union type */
edfmttype_t		*edfmt_add_type_union(char *name, int size)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 1);
  ltype->type = EDFMT_TYPE_UNION;
  ltype->size = size;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

 /* Create an attribute type */
edfmttype_t		*edfmt_add_type_attr(edfmttype_t *tstruct, char *name, 
					     int start, int size, edfmttype_t *type)
{
  edfmttype_t		*ltype, *last = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!tstruct || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 0);
  ltype->type = EDFMT_TYPE_ATTR;
  ltype->parent = tstruct;
  ltype->child = type;
  ltype->start = start;
  ltype->size = size ? size : tstruct->size - start;

  if (type && type->size > 0)
    ltype->size = type->size;

  if (tstruct->child == NULL)
    tstruct->child = ltype;
  else
    {
      /* Iterate to find the end */
      for (last = tstruct->child; last && last->next; last = last->next);

      if (last)
	{
	  if (tstruct->type != EDFMT_TYPE_UNION && last->size == tstruct->size - last->start)
	    last->size = start - last->start;

	  last->next = ltype;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a void type */
edfmttype_t		*edfmt_add_type_void(char *name)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 1);
  ltype->type = EDFMT_TYPE_VOID;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a link type */
edfmttype_t		*edfmt_add_type_link(char *name, edfmttype_t *type)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype, name, 1);
  ltype->type = EDFMT_TYPE_LINK;
  ltype->parent = type;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

edfmtvar_t		*edfmt_add_var_global(edfmttype_t *type, char *name, elfsh_Addr addr)
{
  edfmtvar_t		*lvar;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!type || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_VAR(lvar, name, 1);
  lvar->scope = EDFMT_SCOPE_GLOBAL;
  lvar->addr = addr;
  lvar->type = type;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, lvar);
}

edfmtfunc_t		*edfmt_add_func(char *name, edfmttype_t *ret, 
					elfsh_Addr start, elfsh_Addr end)
{
  edfmtfunc_t		*lfunc;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid paramters", NULL);

  EDFMT_NEW_FUNC(lfunc, name, 1);
  lfunc->rettype = ret;
  lfunc->start = start;
  lfunc->end = end;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, lfunc);
}

edfmtfuncarg_t		*edfmt_add_arg(edfmtfunc_t *func, char *name,
				       u_int reg, int pos, edfmttype_t *type)
{
  edfmtfuncarg_t	*larg, *find;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!func || !name || !type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid paramters", NULL);

  EDFMT_NEW_ARG(larg, name);
  larg->type = type;
  larg->reg = reg;
  larg->pos = pos;

  /* Search last argument */
  for (find = func->arguments; 
       find != NULL && find->next != NULL; 
       find = find->next);
  
  /* Update pointer as needed */
  if (find == NULL)
    func->arguments = larg;
  else
    find->next = larg;
  
  /* Then update arg counting variable */
  func->argc++;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, larg);
}
