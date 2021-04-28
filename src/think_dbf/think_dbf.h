/*
 * think_dbf.h: DBF Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_DBF_H__
#define __THINK_DBF_H__

#include "think.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* open flags */
#define THINK_DBF_CREAT 	0x01		/* creat */
#define THINK_DBF_EXCL 		0x02		/* excl */
#define THINK_DBF_TRUNC 	0x04		/* trunc */
#define THINK_DBF_RDONLY 	0x08		/* read only */
#define THINK_DBF_WRONLY 	0x10		/* write only */
#define THINK_DBF_RDWR 		0x20		/* read & write */
#define THINK_DBF_APPEND 	0x40		/* append */
#define THINK_DBF_SYNC		0x80		/* sync */

/* seek */
#define THINK_DBF_SEEK_SET	0		/* from first record */
#define THINK_DBF_SEEK_CUR	1		/* from current record */
#define THINK_DBF_SEEK_END	2		/* from last record */

/* field type */
#define THINK_DBF_CHAR		'C'		/* 1 bytes */
#define THINK_DBF_SHORT		'S'		/* 2 bytes */
#define THINK_DBF_INT		'I'		/* 4 bytes */
#define THINK_DBF_LONG		'L'		/* 8 bytes */
#define THINK_DBF_FLOAT		'F'		/* 4 bytes */
#define THINK_DBF_DOUBLE	'D'		/* 8 bytes */

/* flags */
#define THINK_DBF_NOT_NULL		0x80000000		/* not null */
#define THINK_DBF_NOT_STRING	0x40000000		/* not string */
#define THINK_DBF_NOT_TRIM		0x20000000		/* not trim */

#define THINK_DBF_EOF		-1		/* eof */

/* dbf field */
struct __think_dbf_field {
	char name[11];				/* field name */
	char type;				/* field type */
	THINK_UINT offset;			/* field offset */
	THINK_UINT length;			/* field length */
};
typedef struct __think_dbf_field THINK_DBF_FIELD;

/* dbf handle */
struct __think_dbf {
	int fd;			/* dbf fd */
	int flags;				/* open flags */
	int cursor;				/* cursor */
	char *buffer;				/* buffer */

	char last_modify_date[11];		/* last modify date: yyyy-mm-dd */
	THINK_UINT record_count;		/* record count */
	THINK_USHORT head_length;		/* head length */
	THINK_USHORT record_length;		/* record length */
	unsigned int field_count;		/* field count */
	THINK_DBF_FIELD *field_array;		/* field array */
};
typedef struct __think_dbf THINK_DBF;

/* think_dbf_open */
extern THINK_DBF *think_dbf_open(const char *pathname,int flags,const char *conffile);

/* think_dbf_fetch */
extern int think_dbf_fetch(THINK_DBF *dbf);

/* think_dbf_getfield */
extern int think_dbf_getfield(THINK_DBF *dbf,unsigned int no,char type,void *value,unsigned int size,int flags);

/* think_dbf_getfieldbyname */
extern int think_dbf_getfieldbyname(THINK_DBF *dbf,const char *name,char type,void *value,unsigned int size,int flags);

/* think_dbf_new */
extern void think_dbf_new(THINK_DBF *dbf);

/* think_dbf_putfield */
extern int think_dbf_putfield(THINK_DBF *dbf,unsigned int no,char type,const void *value,unsigned int length,int flags);

/* think_dbf_putfieldbyname */
extern int think_dbf_putfieldbyname(THINK_DBF *dbf,const char *name,char type,const void *value,unsigned int length,int flags);

/* think_dbf_insert */
extern int think_dbf_insert(THINK_DBF *dbf);

/* think_dbf_seek */
extern int think_dbf_seek(THINK_DBF *dbf,int offset,int whence);

/* think_dbf_sync */
extern int think_dbf_sync(THINK_DBF *dbf);

/* think_dbf_eof */
extern int think_dbf_eof(THINK_DBF *dbf);

/* think_dbf_close */
extern int think_dbf_close(THINK_DBF *dbf);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_DBF_H__ */
