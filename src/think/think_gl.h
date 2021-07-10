/*
 * think_gl.h: Think General Language Interface Version 1 (gl--girl)
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */
#ifndef __THINK_GL_H__
#define __THINK_GL_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_GL_CHAR		'C'		/* 1 bytes */
#define THINK_GL_SHORT		'S'		/* 2 bytes */
#define THINK_GL_INT		'I'		/* 4 bytes */
#define THINK_GL_LONG		'L'		/* 8 bytes */
#define THINK_GL_FLOAT		'F'		/* 4 bytes */
#define THINK_GL_DOUBLE		'D'		/* 8 bytes */

/* flags */
#define THINK_GL_BINARY		0x80000000
#define THINK_GL_UNSIGNED	0x40000000
#define THINK_GL_RECORD		0x20000000	/* data is a single record */
#define THINK_GL_RECORDSET	0x10000000

/* opflags */
#define THINK_GL_IGNORE		0x00008000
#define THINK_GL_NOT_STRING	0x00004000
#define THINK_GL_NOT_TRIM	0x00002000
#define THINK_GL_NOT_NULL	0x00001000	/* not empty on char,not zero on numeric */
#define THINK_GL_FIXLEN		0x00000800	/* length must be a fix */
#define THINK_GL_DEBUG		0x00000400	/* debug mode(send attr info) */
#define THINK_GL_UPDATE		0x00000200	/* update field */

/* attribute no */
#define THINK_GL_NAME		0
#define THINK_GL_TYPE		1
#define THINK_GL_MAXLEN		2		/* integer length on numeric */
#define THINK_GL_FLAGS		3
#define THINK_GL_DESC		4

/* rule */
typedef struct {
	THINK_USHORT no;	/* no */
	char name[32];	/* name */
	char type;		/* type */
	THINK_UINT maxlen;	/* maximal length */
	THINK_INT flags;	/* flags */
	char desc[128];	/* desc */
} THINK_GL_RULE;

/* field */
typedef struct {
	unsigned short no;	/* no */
	void *data;		/* data */
	unsigned int size;	/* size */
	unsigned int *len;	/* length */
	int *exist;	/* exist */
	int opflags;	/* opflags */
} THINK_GL_FIELD;

/* field rule */
typedef struct {
	THINK_USHORT no;	/* no */
	char name[32];	/* name */
	char type;		/* type */
	THINK_UINT maxlen;	/* maximal length */
	THINK_INT flags;	/* flags */
	char desc[128];	/* desc */
	void *data;		/* data */
	unsigned int size;	/* size */
	unsigned int *len;	/* length */
	int *exist;	/* exist */
	int opflags;	/* opflags */
} THINK_GL_FIELDRULE;

/* think_gl_getflags */
int think_gl_getflags(const char *strflags);

/* think_gl_find */
const char *think_gl_find(const char *msgbuf,unsigned int msglen,unsigned short no);

/* think_gl_attr_find */
const char *think_gl_attr_find(const char *attrbuf,unsigned short attrlen,unsigned char attrno);

/* think_gl_attr_get */
int think_gl_attr_get(const char *attrbuf,unsigned short attrlen,unsigned char attrno,void *data,unsigned char size);

/* think_gl_attr_put */
int think_gl_attr_put(char *attrbuf,unsigned short attrsiz,unsigned char attrno,const void *data,unsigned char len);

/* think_gl_record_get */
int think_gl_record_get(const char *msgbuf,unsigned int msglen,char *recbuf,unsigned int recsiz);

/* think_gl_record_put */
int think_gl_record_put(char *msgbuf,unsigned int msgsiz,const char *recbuf,unsigned int reclen);

/* think_gl_record_stat */
int think_gl_record_stat(const char *msgbuf,unsigned int msglen);

/* think_gl_record_fetch */
int think_gl_record_fetch(const char *msgbuf,unsigned int msglen,unsigned int recno,char *recbuf,unsigned int recsiz);

/* think_gl_get */
int think_gl_get(const char *msgbuf,unsigned int msglen,unsigned short no,char *name,char type,unsigned int maxlen,int flags,char *desc,void *data,unsigned int size,int opflags);

/* think_gl_put */
int think_gl_put(char *msgbuf,unsigned int msgsiz,unsigned short no,const char *name,char type,unsigned int maxlen,int flags,const char *desc,const void *data,unsigned int len,int opflags);

/* think_gl_del */
int think_gl_del(char *msgbuf,unsigned int msglen,unsigned short no);

/* think_gl_gets */
int think_gl_gets(const char *msgbuf,unsigned int msglen,const THINK_GL_FIELDRULE *fieldrulearray,unsigned int fieldrulecount,int opflags);

/* think_gl_puts */
int think_gl_puts(char *msgbuf,unsigned int msgsiz,const THINK_GL_FIELDRULE *fieldrulearray,unsigned int fieldrulecount,int opflags);

/* think_gl_autoget */
int think_gl_autoget(const char *msgbuf,unsigned int msglen,unsigned short no,void *data,unsigned int size,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags);

/* think_gl_autoput */
int think_gl_autoput(char *msgbuf,unsigned int msgsiz,unsigned short no,const void *data,unsigned int len,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags);

/* think_gl_autogets */
int think_gl_autogets(const char *msgbuf,unsigned int msglen,const THINK_GL_FIELD *fieldarray,unsigned int fieldcount,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags);

/* think_gl_autoputs */
int think_gl_autoputs(char *msgbuf,unsigned int msgsiz,const THINK_GL_FIELD *fieldarray,unsigned int fieldcount,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags);

/* think_gl_loadrules */
int think_gl_loadrules(const char *file,THINK_GL_RULE **rulearray);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_GL_H__ */
