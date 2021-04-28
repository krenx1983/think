/*
 * think_cfg.h: Think Cfg Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-14	1.0 released
 *
 */
#ifndef __THINK_CFG_H__
#define __THINK_CFG_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_CFG_FIELD_SIZE	128	/* field size */
#define THINK_CFG_NAME_SIZE	128	/* name size */
#define THINK_CFG_VALUE_SIZE	1024	/* value size */
#define THINK_CFG_FILE_SIZE	256	/* file size */

struct __think_cfg_cfglist {
	char field[THINK_CFG_FIELD_SIZE];	/* field */
	char name[THINK_CFG_NAME_SIZE];		/* name */
	char value[THINK_CFG_VALUE_SIZE];	/* value */
	char file[THINK_CFG_FILE_SIZE];		/* file */
	unsigned int lineno;
	struct __think_cfg_cfglist *prior;
	struct __think_cfg_cfglist *next;
};			/* cfg list */
typedef struct __think_cfg_cfglist THINK_CFG_CFGLIST;

struct __think_cfg{
	unsigned int cfgcount;		/* cfg count */
	THINK_CFG_CFGLIST *cfglist; 	/* cfg list */
};
typedef struct __think_cfg THINK_CFG;

#define THINK_CFG_CHAR			'C'		/* 1 bytes */
#define THINK_CFG_SHORT			'S'		/* 2 bytes */
#define THINK_CFG_INT			'I'		/* 4 bytes */
#define THINK_CFG_LONG			'L'		/* 8 bytes */
#define THINK_CFG_FLOAT			'F'		/* 4 bytes */
#define THINK_CFG_DOUBLE		'D'		/* 8 bytes */

#define THINK_CFG_NOT_IGNORE	0x80000000		/* not ignore */
#define THINK_CFG_NOT_NULL		0x40000000		/* not null */
#define THINK_CFG_NOT_STRING	0x20000000		/* not string */

struct __think_cfgarray {
	const char *domain;
	const char *name;
	char type;
	void *value;
	unsigned int size;
	int flags;
	int *exists;
};		/* cfg array */
typedef struct __think_cfgarray THINK_CFGARRAY;

/* load cfg */
THINK_CFG *think_loadcfg(const char *file,const char *path);

/* get cfg */
int think_getcfg(THINK_CFG *cfg,const char *field,const char *name,char type,void *value,unsigned int size,int flags);

/* show cfg */
int think_showcfg(THINK_CFG *cfg);

/* free cfg */
int think_freecfg(THINK_CFG *cfg);

/* get cfgs */
int think_getcfgs(THINK_CFG *cfg,THINK_CFGARRAY *cfgarray,unsigned int cfgcount);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_CFG_H__ */
