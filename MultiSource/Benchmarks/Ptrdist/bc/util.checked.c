/* util.c: Utility routines for bc. */

/*  This file is part of bc written for MINIX.
    Copyright (C) 1991, 1992 Free Software Foundation, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to
    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

    You may contact the author by:
       e-mail:  phil@cs.wwu.edu
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062
       
*************************************************************************/

#include "bcdefs.h"
#include <stdarg.h>
#include "global.h"
#include "proto.h"


/* strcopyof mallocs new memory and copies a string to to the new
   memory. */

char *strcopyof(char *str : itype(_Ptr<char>)) : itype(_Ptr<char>)
{
  char * temp;

  temp = (char *)malloc<char>(strlen (str)+1);
  strcpy (temp,str);
  return (temp);
}


/* nextarg adds another value to the list of arguments. */

_Ptr<arg_list> nextarg(arg_list *args : itype(_Ptr<arg_list>), char val)
{ _Ptr<arg_list> temp = ((void *)0);

  temp = (_Ptr<arg_list>)malloc<arg_list>(sizeof (arg_list));
  temp->av_name = val;
  temp->next = args;
 
  return (temp);
}


/* For generate, we must produce a string in the form
    "val,val,...,val".  We also need a couple of static variables
   for retaining old generated strings.  It also uses a recursive
   function that builds the string. */

static _Ptr<char> arglist1 = ((void *)0);
static _Ptr<char> arglist2 = ((void *)0);


/* make_arg_str does the actual construction of the argument string.
   ARGS is the pointer to the list and LEN is the maximum number of
   characters needed.  1 char is the minimum needed. COMMAS tells
   if each number should be seperated by commas.*/

static char *make_arg_str(_Ptr<arg_list> args, int len, int commas) : itype(_Ptr<char>);

static char *make_arg_str(_Ptr<arg_list> args, int len, int commas) : itype(_Ptr<char>)
{
  char * temp;
  char sval[20];

  /* Recursive call. */
  if (args != NULL)
    temp = make_arg_str (_Assume_bounds_cast<_Ptr<arg_list>>(args->next), len+11, commas);
  else
    {
      temp = (char *)malloc<char>(len);
      *temp = 0;
      return temp;
    }

  /* Add the current number to the end of the string. */
  if (len != 1 && commas) 
    sprintf (sval, "%d,", args->av_name);
  else
    sprintf (sval, "%d", args->av_name);
  /* XXX temp = */ strcat (temp, sval);
  return (temp);
}

_Ptr<char> arg_str(_Ptr<arg_list> args, int commas)
{
  if (arglist2 != NULL) 
    free<char>(arglist2);
  arglist2 = arglist1;
  arglist1 = make_arg_str (args, 1, commas);
  return (arglist1);
}


/* free_args frees an argument list ARGS. */

void
free_args (arg_list *args : itype(_Ptr<arg_list>))
{ 
  arg_list * temp;
 
  temp = args;
  while (temp != NULL)
    {
      args = args->next;
      free<arg_list>(temp);
      temp = args;
    }
}


/* Check for valid parameter (PARAMS) and auto (AUTOS) lists.
   There must be no duplicates any where.  Also, this is where
   warnings are generated for array parameters. */

void
check_params (arg_list *params : itype(_Ptr<arg_list>), arg_list *autos : itype(_Ptr<arg_list>))
{
  arg_list * tmp1;
  arg_list * tmp2;

  /* Check for duplicate parameters. */
  if (params != NULL)
    {
      tmp1 = params;
      while (tmp1 != NULL)
	{
	  tmp2 = tmp1->next;
	  while (tmp2 != NULL)
	    {
	      if (tmp2->av_name == tmp1->av_name) 
		yyerror ("duplicate parameter names");
	      tmp2 = tmp2->next;
	    }
	  if (tmp1->av_name < 0)
	    warn ("Array parameter");
	  tmp1 = tmp1->next;
	}
    }

  /* Check for duplicate autos. */
  if (autos != NULL)
    {
      tmp1 = autos;
      while (tmp1 != NULL)
	{
	  tmp2 = tmp1->next;
	  while (tmp2 != NULL)
	    {
	      if (tmp2->av_name == tmp1->av_name) 
		yyerror ("duplicate auto variable names");
	      tmp2 = tmp2->next;
	    }
	  tmp1 = tmp1->next;
	}
    }

  /* Check for duplicate between parameters and autos. */
  if ((params != NULL) && (autos != NULL))
    {
      tmp1 = params;
      while (tmp1 != NULL)
	{
	  tmp2 = autos;
	  while (tmp2 != NULL)
	    {
	      if (tmp2->av_name == tmp1->av_name) 
		yyerror ("variable in both parameter and auto lists");
	      tmp2 = tmp2->next;
	    }
	  tmp1 = tmp1->next;
	}
    }
}


/* Initialize the code generator the parser. */

void
init_gen (void)
{
  /* Get things ready. */
  break_label = 0;
  continue_label = 0;
  next_label  = 1;
  out_count = 2;
  if (compile_only) 
    printf ("@i");
  else
    init_load ();
  had_error = FALSE;
  did_gen = FALSE;
}


/* generate code STR for the machine. */

void
generate (char *str : itype(_Ptr<char>))
{
  did_gen = TRUE;
  if (compile_only)
    {
      printf ("%s",str);
      out_count += strlen(str);
      if (out_count > 60)
	{
	  printf ("\n");
	  out_count = 0;
	}
    }
  else
    load_code (str);
}


/* Execute the current code as loaded. */

void
run_code(void)
{
  /* If no compile errors run the current code. */
  if (!had_error && did_gen)
    {
      if (compile_only)
	{
	  printf ("@r\n"); 
	  out_count = 0;
	}
      else
	execute ();
    }

  /* Reinitialize the code generation and machine. */
  if (did_gen)
    init_gen();
  else
    had_error = FALSE;
}


/* Output routines: Write a character CH to the standard output.
   It keeps track of the number of characters output and may
   break the output with a "\<cr>". */

void
out_char (char ch)
{
  if (ch == '\n')
    {
      out_col = 0;
      putchar ('\n');
    }
  else
    {
      out_col++;
      if (out_col == 70)
	{
	  putchar ('\\');
	  putchar ('\n');
	  out_col = 1;
	}
      putchar (ch);
    }
}


/* The following are "Symbol Table" routines for the parser. */

/*  find_id returns a pointer to node in TREE that has the correct
    ID.  If there is no node in TREE with ID, NULL is returned. */

_Ptr<id_rec> find_id(_Ptr<id_rec> tree, char *id : itype(_Ptr<char>))
{
  int cmp_result;
  
  /* Check for an empty tree. */
  if (tree == NULL)
    return NULL;

  /* Recursively search the tree. */
  cmp_result = strcmp (id, tree->id);
  if (cmp_result == 0)
    return tree;  /* This is the item. */
  else if (cmp_result < 0)
    return find_id (_Assume_bounds_cast<_Ptr<id_rec>>(tree->left), id);
  else
    return find_id (_Assume_bounds_cast<_Ptr<id_rec>>(tree->right), id);  
}


/* insert_id_rec inserts a NEW_ID rec into the tree whose ROOT is
   provided.  insert_id_rec returns TRUE if the tree height from
   ROOT down is increased otherwise it returns FALSE.  This is a
   recursive balanced binary tree insertion algorithm. */

int insert_id_rec (_Ptr<id_rec *> root, id_rec *new_id : itype(_Ptr<id_rec>))
{
  id_rec * A;
  id_rec * B;

  /* If root is NULL, this where it is to be inserted. */
  if (*root == NULL)
    {
      *root = new_id;
      new_id->left = NULL;
      new_id->right = NULL;
      new_id->balance = 0;
      return (TRUE);
    }

  /* We need to search for a leaf. */
  if (strcmp (new_id->id, (*root)->id) < 0)
    {
      /* Insert it on the left. */
      if (insert_id_rec (&(*root)->left, new_id))
	{
	  /* The height increased. */
	  (*root)->balance --;
	  
      switch ((*root)->balance)
	{
	case  0:  /* no height increase. */
	  return (FALSE);
	case -1:  /* height increase. */
	  return (FALSE);
	case -2:  /* we need to do a rebalancing act. */
	  A = *root;
	  B = (*root)->left;
	  if (B->balance <= 0)
	    {
	      /* Single Rotate. */
	      A->left = B->right;
	      B->right = A;
	      *root = B;
	      A->balance = 0;
	      B->balance = 0;
	    }
	  else
	    {
	      /* Double Rotate. */
	      *root = B->right;
	      B->right = (*root)->left;
	      A->left = (*root)->right;
	      (*root)->left = B;
	      (*root)->right = A;
	      switch ((*root)->balance)
		{
		case -1:
		  A->balance = 1;
		  B->balance = 0;
		  break;
		case  0:
		  A->balance = 0;
		  B->balance = 0;
		  break;
		case  1:
		  A->balance = 0;
		  B->balance = -1;
		  break;
		}
	      (*root)->balance = 0;
	    }
	}     
	} 
    }
  else
    {
      /* Insert it on the right. */
      if (insert_id_rec (&(*root)->right, new_id))
	{
	  /* The height increased. */
	  (*root)->balance ++;
	  switch ((*root)->balance)
	    {
	    case 0:  /* no height increase. */
	      return (FALSE);
	    case 1:  /* height increase. */
	      return (FALSE);
	    case 2:  /* we need to do a rebalancing act. */
	      A = *root;
	      B = (*root)->right;
	      if (B->balance >= 0)
		{
		  /* Single Rotate. */
		  A->right = B->left;
		  B->left = A;
		  *root = B;
		  A->balance = 0;
		  B->balance = 0;
		}
	      else
		{
		  /* Double Rotate. */
		  *root = B->left;
		  B->left = (*root)->right;
		  A->right = (*root)->left;
		  (*root)->left = A;
		  (*root)->right = B;
		  switch ((*root)->balance)
		    {
		    case -1:
		      A->balance = 0;
		      B->balance = 1;
		      break;
		    case  0:
		      A->balance = 0;
		      B->balance = 0;
		      break;
		    case  1:
		      A->balance = -1;
		      B->balance = 0;
		      break;
		    }
		  (*root)->balance = 0;
		}
	    }     
	} 
    }
  
  /* If we fall through to here, the tree did not grow in height. */
  return (FALSE);
}


/* Initialize variables for the symbol table tree. */

void
init_tree(void)
{
  name_tree  = NULL;
  next_array = 1;
  next_func  = 1;
  next_var   = 4;  /* 0 => ibase, 1 => obase, 2 => scale, 3 => last. */
}


/* Lookup routines for symbol table names. */

int
lookup (char *name : itype(_Ptr<char>), int namekind)
{
  _Ptr<id_rec> id = ((void *)0);

  /* Warn about non-standard name. */
  if (strlen(name) != 1)
    warn ("multiple letter name - %s", name);

  /* Look for the id. */
  id = find_id (_Assume_bounds_cast<_Ptr<id_rec>>(name_tree), name);
  if (id == NULL)
    {
      /* We need to make a new item. */
      id = (_Ptr<id_rec>)malloc<id_rec>(sizeof (id_rec));
      id->id = strcopyof (name);
      id->a_name = 0;
      id->f_name = 0;
      id->v_name = 0;
      insert_id_rec (&(name_tree), id);
    }

  /* Return the correct value. */
  switch (namekind)
    {
      
    case ARRAY:
      /* ARRAY variable numbers are returned as negative numbers. */
      if (id->a_name != 0)
	{
	  free<char>(name);
	  return (-id->a_name);
	}
      id->a_name = next_array++;
      a_names[id->a_name] = name;
      if (id->a_name < MAX_STORE)
	{
	  if (id->a_name >= a_count)
	    more_arrays ();
	  return (-id->a_name);
	}
      yyerror ("Too many array variables");
      exit (1);

    case FUNCT:
      if (id->f_name != 0)
	{
	  free<char>(name);
	  return (id->f_name);
	}
      id->f_name = next_func++;
      f_names[id->f_name] = name;
      if (id->f_name < MAX_STORE)
	{
	  if (id->f_name >= f_count)
	    more_functions ();
	  return (id->f_name);
	}
      yyerror ("Too many functions");
      exit (1);

    case SIMPLE:
      if (id->v_name != 0)
	{
	  free<char>(name);
	  return (id->v_name);
	}
      id->v_name = next_var++;
      v_names[id->v_name - 1] = name;
      if (id->v_name <= MAX_STORE)
	{
	  if (id->v_name >= v_count)
	    more_variables ();
	  return (id->v_name);
	}
      yyerror ("Too many variables");
      exit (1);
    }
}


/* Print the welcome banner. */

void 
welcome(void)
{
  printf ("This is free software with ABSOLUTELY NO WARRANTY.\n");
  printf ("For details type . \n");
}


/* Print out the warranty information. */

void 
warranty(char *prefix : itype(_Ptr<char>))
{
  printf ("\n%s%s\n\n", prefix, BC_VERSION);
  printf ("%s%s%s%s%s%s%s%s%s%s%s",
"    This program is free software; you can redistribute it and/or modify\n",
"    it under the terms of the GNU General Public License as published by\n",
"    the Free Software Foundation; either version 2 of the License , or\n",
"    (at your option) any later version.\n\n",
"    This program is distributed in the hope that it will be useful,\n",
"    but WITHOUT ANY WARRANTY; without even the implied warranty of\n",
"    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n",
"    GNU General Public License for more details.\n\n",
"    You should have received a copy of the GNU General Public License\n",
"    along with this program. If not, write to the Free Software\n",
"    Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.\n\n");
}

/* Print out the limits of this program. */

void
limits(void)
{
  printf ("BC_BASE_MAX     = %d\n",  BC_BASE_MAX);
  printf ("BC_DIM_MAX      = %ld\n", (long) BC_DIM_MAX);
  printf ("BC_SCALE_MAX    = %d\n",  BC_SCALE_MAX);
  printf ("BC_STRING_MAX   = %d\n",  BC_STRING_MAX);
  printf ("MAX Exponent    = %ld\n", (long) LONG_MAX);
  printf ("MAX code        = %ld\n", (long) BC_MAX_SEGS * (long) BC_SEG_SIZE);
  printf ("multiply digits = %ld\n", (long) LONG_MAX / (long) 90);
  printf ("Number of vars  = %ld\n", (long) MAX_STORE);
#ifdef OLD_EQ_OP
  printf ("Old assignment operatiors are valid. (=-, =+, ...)\n");
#endif 
}


#ifdef notdef

/* bc_malloc will check the return value so all other places do not
   have to do it!  SIZE is the number of types to allocate. */

char *
bc_malloc (size)
     int size;
{
  char *ptr;

  ptr = (char *) malloc (size);
  if (ptr == NULL)
    out_of_memory ();

  return ptr;
}


/* The following routines are error routines for various problems. */

/* Malloc could not get enought memory. */

void
out_of_memory()
{
  fprintf (stderr, "Fatal error: Out of memory for malloc.\n");
  exit (1);
}

#endif


/* The standard yyerror routine.  Built with variable number of argumnets. */

void
yyerror (char *str : itype(_Ptr<char>), ...)
{
  char *name;
  va_list args;

  va_start (args, str);

  if (is_std_in)
    name = "(standard_in)";
  else
    name = g_argv[optind-1];
  fprintf (stderr,"%s %d: ",name,line_no);
  vfprintf (stderr, str, args);
  fprintf (stderr, "\n");
  had_error = TRUE;
  va_end (args);
}


/* The routine to produce warnings about non-standard features
   found during parsing. */

void
warn (char *mesg : itype(_Ptr<char>), ...)
{
  char *name;
  va_list args;

  va_start (args, mesg);

  if (std_only)
    {
      if (is_std_in)
	name = "(standard_in)";
      else
	name = g_argv[optind-1];
      fprintf (stderr,"%s %d: ",name,line_no);
      vfprintf (stderr, mesg, args);
      fprintf (stderr, "\n");
      had_error = TRUE;
    }
  else
    if (warn_not_std)
      {
	if (is_std_in)
	  name = "(standard_in)";
	else
	  name = g_argv[optind-1];
	fprintf (stderr,"%s %d: (Warning) ",name,line_no);
	vfprintf (stderr, mesg, args);
	fprintf (stderr, "\n");
      }
  va_end (args);
}

/* Runtime error will  print a message and stop the machine. */

void
rt_error (char *mesg : itype(_Ptr<char>), ...)
{
  va_list args;
  char error_mesg [255];

  va_start (args, mesg);

  vsprintf (error_mesg, mesg, args);
  va_end (args);
  
  fprintf (stderr, "Runtime error (func=%s, adr=%d): %s\n",
	   f_names[pc.pc_func], pc.pc_addr, error_mesg);
  runtime_error = TRUE;
}


/* A runtime warning tells of some action taken by the processor that
   may change the program execution but was not enough of a problem
   to stop the execution. */

void
rt_warn (char *mesg : itype(_Ptr<char>), ...)
{
  va_list args;
  char error_mesg [255];

  va_start (args, mesg);

  vsprintf (error_mesg, mesg, args);
  va_end (args);

  fprintf (stderr, "Runtime warning (func=%s, adr=%d): %s\n",
	   f_names[pc.pc_func], pc.pc_addr, error_mesg);
}
