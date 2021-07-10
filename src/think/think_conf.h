/*
 * think_conf.h: Think Conf Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-14	1.0 released
 *
 * examples:
 * dbf.conf:
 * Name		|Type	|Length	|Desc
 * ID		|int	|3	|ID
 * Name		|char	|15	|Name
 * Email	|char	|30	|Email
 *
 * server.c:
 * CONF *conf=NULL;
 * char value[128];
 * conf=think_confload("table.conf",'|');
 * confshow(conf);
 * bzero(value,sizeof(value));
 * confget(conf,0,0,value,sizeof(value));
 * conffree(conf);
 *
 */
#ifndef __THINK_CONF_H__
#define __THINK_CONF_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* field type */
#define THINK_CONF_CHAR		'C'		/* 1 bytes */
#define THINK_CONF_SHORT	'S'		/* 2 bytes */
#define THINK_CONF_INT		'I'		/* 4 bytes */
#define THINK_CONF_LONG		'L'		/* 8 bytes */
#define THINK_CONF_FLOAT	'F'		/* 4 bytes */
#define THINK_CONF_DOUBLE	'D'		/* 8 bytes */

#define THINK_CONF_NOT_NULL		0x80000000		/* not null */
#define THINK_CONF_NOT_STRING	0x40000000		/* not string */

#define THINK_CONF_VALUE_SIZE	1024
#define THINK_CONF_FILE_SIZE	256

struct __think_conf_vlist {
	char value[THINK_CONF_VALUE_SIZE];	/* column value */
	struct __think_conf_vlist *prior;
	struct __think_conf_vlist *next;
};	/* column list */
typedef struct __think_conf_vlist THINK_CONF_VLIST;

struct __think_conf_hlist {
	struct __think_conf_vlist *vlist;		/* column list */
	struct __think_conf_hlist *prior;
	struct __think_conf_hlist *next;
};	/* line list */
typedef struct __think_conf_hlist THINK_CONF_HLIST;

struct __think_conf {
	char d;					/* column delimiter */
	unsigned int hcount;			/* line count */
	unsigned int vcount;			/* column count */
	char file[THINK_CONF_FILE_SIZE];	/* file */
	THINK_CONF_HLIST *hlist;		/* line list */
};
typedef struct __think_conf THINK_CONF;

THINK_CONF *think_loadconf(const char *file,char d);
int think_getconf(THINK_CONF *conf,int h,int v,char type,void *value,unsigned int size,int flags);               
int think_showconf(THINK_CONF *conf);
int think_freeconf(THINK_CONF *conf);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_CONF_H__ */
