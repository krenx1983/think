/*
 * think_sem.h: Think Semaphore Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_SEM_H__
#define __THINK_SEM_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_SEM_CREAT		0x80000000
#define THINK_SEM_TRUNC		0x40000000
#define THINK_SEM_EXCL		0x20000000
#define THINK_SEM_UNDO		0x10000000
#define THINK_SEM_RUSR		0x00000100		/*0400(r)*/
#define THINK_SEM_WUSR		0x00000080		/*0200(w)*/
#define THINK_SEM_XUSR		0x00000040		/*0100(x)*/
#define THINK_SEM_RGRP		0x00000020		/*0040(r)*/
#define THINK_SEM_WGRP		0x00000010		/*0020(w)*/
#define THINK_SEM_XGRP		0x00000008		/*0010(x)*/
#define THINK_SEM_ROTH		0x00000004		/*0004(r)*/
#define THINK_SEM_WOTH		0x00000002		/*0002(w)*/
#define THINK_SEM_XOTH		0x00000001		/*0001(x)*/

#define THINK_SEM_MASK_SECURITY	0x000001FF	/*0777(rwxrwxrwx)*/

struct __think_sem{
#ifdef __THINK_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int semid;
#endif
	int flags;
};
typedef struct __think_sem THINK_SEM;

#ifdef __THINK_UNIX__ /* UNIX */
union __think_semun{
	int val;
	struct semid_ds *buf;
	ushort *array;
#ifdef __THINK_LINUX__ /* linux */
	struct seminfo *__buf;
#endif /* __THINK_LINUX__ */
};
typedef union __think_semun THINK_SEMUN;
#endif /* __THINK_UNIX__ */

THINK_SEM *think_semopen(const char *name,int flags);
int think_semget(THINK_SEM *sem,int timeout);
int think_semput(THINK_SEM *sem);
int think_semclose(THINK_SEM *sem);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_SEM_H__ */
