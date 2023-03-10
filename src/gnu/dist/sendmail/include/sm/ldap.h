/* $NetBSD: ldap.h,v 1.1.1.3 2005/03/15 02:05:52 atatat Exp $ */
/*
 * Copyright (c) 2001-2002 Sendmail, Inc. and its suppliers.
 *      All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 *	Id: ldap.h,v 1.27 2003/12/20 09:23:47 gshapiro Exp
 */

#ifndef	SM_LDAP_H
# define SM_LDAP_H

# include <sm/conf.h>
# include <sm/rpool.h>

/*
**  NOTE: These should be changed from LDAPMAP_* to SM_LDAP_*
**        in the next major release (8.13) of sendmail.
*/

# ifndef LDAPMAP_MAX_ATTR
#  define LDAPMAP_MAX_ATTR	64
# endif /* ! LDAPMAP_MAX_ATTR */
# ifndef LDAPMAP_MAX_FILTER
#  define LDAPMAP_MAX_FILTER	1024
# endif /* ! LDAPMAP_MAX_FILTER */
# ifndef LDAPMAP_MAX_PASSWD
#  define LDAPMAP_MAX_PASSWD	256
# endif /* ! LDAPMAP_MAX_PASSWD */

# if LDAPMAP

/* Attribute types */
#  define SM_LDAP_ATTR_NONE		(-1)
#  define SM_LDAP_ATTR_OBJCLASS	0
#  define SM_LDAP_ATTR_NORMAL		1
#  define SM_LDAP_ATTR_DN		2
#  define SM_LDAP_ATTR_FILTER		3
#  define SM_LDAP_ATTR_URL		4

/* sm_ldap_results() flags */
#  define SM_LDAP_SINGLEMATCH	0x0001
#  define SM_LDAP_MATCHONLY	0x0002
#  define SM_LDAP_USE_ALLATTR	0x0004

struct sm_ldap_struct
{
	/* needed for ldap_open or ldap_init */
	char		*ldap_uri;
	char		*ldap_host;
	int		ldap_port;
	int		ldap_version;
	pid_t		ldap_pid;

	/* options set in ld struct before ldap_bind_s */
	int		ldap_deref;
	time_t		ldap_timelimit;
	int		ldap_sizelimit;
	int		ldap_options;

	/* args for ldap_bind_s */
	LDAP		*ldap_ld;
	char		*ldap_binddn;
	char		*ldap_secret;
	int		ldap_method;

	/* args for ldap_search */
	char		*ldap_base;
	int		ldap_scope;
	char		*ldap_filter;
	char		*ldap_attr[LDAPMAP_MAX_ATTR + 1];
	int		ldap_attr_type[LDAPMAP_MAX_ATTR + 1];
	char		*ldap_attr_needobjclass[LDAPMAP_MAX_ATTR + 1];
	bool		ldap_attrsonly;

	/* args for ldap_result */
	struct timeval	ldap_timeout;
	LDAPMessage	*ldap_res;

	/* ldapmap_lookup options */
	char		ldap_attrsep;

	/* Linked list of maps sharing the same LDAP binding */
	void		*ldap_next;
};

typedef struct sm_ldap_struct		SM_LDAP_STRUCT;

struct sm_ldap_recurse_entry
{
	char *lr_search;
	int lr_type;
	LDAPURLDesc *lr_ludp;
	char **lr_attrs;
	bool lr_done;
};

struct sm_ldap_recurse_list
{
	int lr_size;
	int lr_cnt;
	struct sm_ldap_recurse_entry **lr_data;
};

typedef struct sm_ldap_recurse_entry	SM_LDAP_RECURSE_ENTRY;
typedef struct sm_ldap_recurse_list	SM_LDAP_RECURSE_LIST;

/* functions */
extern void	sm_ldap_clear __P((SM_LDAP_STRUCT *));
extern bool	sm_ldap_start __P((char *, SM_LDAP_STRUCT *));
extern int	sm_ldap_search __P((SM_LDAP_STRUCT *, char *));
extern int	sm_ldap_results __P((SM_LDAP_STRUCT *, int, int, int,
				     SM_RPOOL_T *, char **, int *, int *,
				     SM_LDAP_RECURSE_LIST *));
extern void	sm_ldap_setopts __P((LDAP *, SM_LDAP_STRUCT *));
extern int	sm_ldap_geterrno __P((LDAP *));
extern void	sm_ldap_close __P((SM_LDAP_STRUCT *));

/* Portability defines */
#  if !SM_CONF_LDAP_MEMFREE
#   define ldap_memfree(x)	((void) 0)
#  endif /* !SM_CONF_LDAP_MEMFREE */

# endif /* LDAPMAP */
#endif /* ! SM_LDAP_H */
