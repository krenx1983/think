/*
 * think_db.h: Think DB Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_DB_H__
#define __THINK_DB_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* open flags */
#define THINK_DB_CREAT 		0x80000000		/* creat */
#define THINK_DB_EXCL 		0x40000000		/* excl */
#define THINK_DB_TRUNC 		0x20000000		/* trunc */
#define THINK_DB_RDONLY 	0x10000000		/* read only */
#define THINK_DB_WRONLY 	0x08000000		/* write only */
#define THINK_DB_RDWR 		0x04000000		/* read & write */
#define THINK_DB_APPEND 	0x02000000		/* append */
#define THINK_DB_SYNC		0x01000000		/* sync */

/* seek */
#define THINK_DB_SEEK_SET	0		/* from first record */
#define THINK_DB_SEEK_CUR	1		/* from current record */
#define THINK_DB_SEEK_END	2		/* from last record */

/* field type */
#define THINK_DB_CHAR		'C'		/* 1 bytes */
#define THINK_DB_SHORT		'S'		/* 2 bytes */
#define THINK_DB_INT		'I'		/* 4 bytes */
#define THINK_DB_LONG		'L'		/* 8 bytes */
#define THINK_DB_FLOAT		'F'		/* 4 bytes */
#define THINK_DB_DOUBLE		'D'		/* 8 bytes */

/* flags */
#define THINK_DB_NOT_NULL	0x80000000		/* not null */
#define THINK_DB_NOT_STRING	0x40000000		/* not string */
#define THINK_DB_NOT_TRIM	0x20000000		/* not trim */

#define THINK_DB_EOF		-1		/* eof */

/* db field */
struct __think_db_field {
	char name[256];				/* name */
	char type;					/* type */
	THINK_UINT length;		/* length */
	THINK_UINT flags;			/* flags */
};
typedef struct __think_db_field THINK_DB_FIELD;

/* db handle */
struct __think_db {
	int fd;							/* db fd */
	unsigned char type;				/* db type */
	unsigned char version;			/* db version */
	int flags;						/* open flags */
	unsigned char field_count;		/* field count */
	THINK_DB_FIELD *field_array;	/* field array */
	char created_time[20];			/* created time */
	char last_modified_time[20];	/* last modified time */
	THINK_UINT record_count;		/* record count */
	unsigned char transaction_flag;	/* transaction flag */
	THINK_UINT option_length;		/* option length */
	char *option_buffer;			/* option buffer */

	THINK_UINT head_length;		/* head length */
	char opflag;					/* operation flag */
	int cursor;						/* cursor */
	char *record_buffer;			/* record buffer */
	unsigned int record_length;		/* record length */
	unsigned int *offset_array;		/* field offset array */
};
typedef struct __think_db THINK_DB;

/* think_db_open */
THINK_DB *think_db_open(const char *pathname,int flags,THINK_DB_FIELD *fieldarray,unsigned int fieldcount);
THINK_DB *think_db_openx(const char *pathname,int flags,const char *conffile);

/* think_db_fetch */
int think_db_fetch(THINK_DB *db);

/* think_db_getfield */
int think_db_getfield(THINK_DB *db,unsigned int no,void *value,unsigned int size,int flags);

/* think_db_getfieldbyname */
int think_db_getfieldbyname(THINK_DB *db,const char *name,void *value,unsigned int size,int flags);

/* think_db_new */
void think_db_new(THINK_DB *db);

/* think_db_putfield */
int think_db_putfield(THINK_DB *db,unsigned int no,const void *value,unsigned int length,int flags);

/* think_db_putfieldbyname */
int think_db_putfieldbyname(THINK_DB *db,const char *name,const void *value,unsigned int length,int flags);

/* think_db_insert */
int think_db_insert(THINK_DB *db);

/* think_db_delete */
int think_db_delete(THINK_DB *db);

/* think_db_seek */
int think_db_seek(THINK_DB *db,int offset,int whence);

/* think_db_sync */
int think_db_sync(THINK_DB *db);

/* think_db_eof */
int think_db_eof(THINK_DB *db);

/* think_db_close */
int think_db_close(THINK_DB *db);

/* think_db_getflags */
int think_db_getfieldflags(const char *strflags);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_DB_H__ */
