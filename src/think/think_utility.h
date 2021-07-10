/*
 * think_utility.h: Think Utility Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_UTILITY_H__
#define __THINK_UTILITY_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

int think_bin_to_hex(char *dest,const char *src,int len);
int think_bin_to_bcd(char *dest,const char *src,int len);
int think_hex_to_bin(char *dest,const char *src,int len);
int think_bcd_to_bin(char *dest,const char *src,int len);
int think_hextobin(char *dest,const char *src);
int think_bcdtobin(char *dest,const char *src);
int think_bintohex(char *dest,char src);
int think_bintobcd(char *dest,char src);
int think_inttohex(int bin);
int think_inttobcd(int bin);
int think_hextoint(int hex);
int think_bcdtoint(int hex);
int think_printbin(const char *buf,const int len,int linesize);

/* string */

#define THINK_STR_CHAR		'C'		/* 1 bytes */
#define THINK_STR_SHORT		'S'		/* 2 bytes */
#define THINK_STR_INT		'I'		/* 4 bytes */
#define THINK_STR_LONG		'L'		/* 8 bytes */
#define THINK_STR_FLOAT		'F'		/* 4 bytes */
#define THINK_STR_DOUBLE	'D'		/* 8 bytes */

#define THINK_STR_NOT_NULL		0x80000000	/* not null */
#define THINK_STR_NOT_STRING	0x40000000	/* not string */
#define THINK_STR_NOT_TRIM		0x20000000	/* not trim */

char *think_strtrim(char *str);
char *think_strinsert(char *srcstr,int pos,char *insstr);
char *think_chrinsert(char *srcstr,int pos,char insstr);
char *think_strreplace(char *srcstr,char *pattern,char *repstr);
char *think_chrreplace(char *srcstr,char pattern,char repstr);
char *think_strdelete(char *srcstr,const char *pattern);
char *think_chrdelete(char *srcstr,char delchr);
int think_strfind(const char *str,const char *pattern,int index);
int think_chrfind(const char *str,char chr,int index);
int think_chrstat(const char *str,char ch);
int think_strstat(const char *str,const char *pattern);

int think_strgetfield(const char *str,char sepchr,int index,char type,void *value,unsigned int size,int flags);

int think_bitset(char *bitmap,int bit);
int think_bittest(const char *bitmap,int bit);
int think_bitclear(char *bitmap,int bit);

#define THINK_OPT_CHAR		'C'		/* 1 bytes */
#define THINK_OPT_SHORT		'S'		/* 2 bytes */
#define THINK_OPT_INT		'I'		/* 4 bytes */
#define THINK_OPT_LONG		'L'		/* 8 bytes */
#define THINK_OPT_FLOAT		'F'		/* 4 bytes */
#define THINK_OPT_DOUBLE	'D'		/* 8 bytes */

#define THINK_OPT_ARG			0x80000000
#define THINK_OPT_NOT_IGNORE	0x40000000
#define THINK_OPT_NOT_NULL		0x20000000
#define THINK_OPT_NOT_STRING	0x10000000
#define THINK_OPT_NOT_TRIM		0x08000000

struct __think_optarray {
	char opt;
	char type;
	void *value;
	unsigned int size;
	int flags;
	int *exists;			/* 0:not exists,1:exists */
};
typedef struct __think_optarray THINK_OPTARRAY;
int think_getopt(int argc,char *argv[],char opt,char type,void *value,unsigned int size,int flags);
int think_getopts(int argc,char *argv[],THINK_OPTARRAY *optarray,unsigned int optcount);

#define THINK_ENV_CHAR		'C'		/* 1 bytes */
#define THINK_ENV_SHORT		'S'		/* 2 bytes */
#define THINK_ENV_INT		'I'		/* 4 bytes */
#define THINK_ENV_LONG		'L'		/* 8 bytes */
#define THINK_ENV_FLOAT		'F'		/* 4 bytes */
#define THINK_ENV_DOUBLE	'D'		/* 8 bytes */

#define THINK_ENV_NOT_IGNORE	0x80000000
#define THINK_ENV_NOT_NULL		0x40000000
#define THINK_ENV_NOT_STRING	0x20000000
#define THINK_ENV_NOT_TRIM		0x10000000

struct __think_envarray {
	const char *name;
	char type;
	void *value;
	unsigned int size;
	int flags;
	int *exists;
};
typedef struct __think_envarray THINK_ENVARRAY;
int think_getenv(const char *name,char type,void *value,int size,int flags);
int think_getenvs(THINK_ENVARRAY *envarray,unsigned int envcount);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_UTILITY_H__ */
