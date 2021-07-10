/*
 * think_mutex.h: Think Mutex Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_MUTEX_H__
#define __THINK_MUTEX_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_MUTEX_CREAT		0x80000000
#define THINK_MUTEX_TRUNC		0x40000000
#define THINK_MUTEX_EXCL		0x20000000
#define THINK_MUTEX_RUSR		0x00000100		/*0400(r)*/
#define THINK_MUTEX_WUSR		0x00000080		/*0200(w)*/
#define THINK_MUTEX_XUSR		0x00000040		/*0100(x)*/
#define THINK_MUTEX_RGRP		0x00000020		/*0040(r)*/
#define THINK_MUTEX_WGRP		0x00000010		/*0020(w)*/
#define THINK_MUTEX_XGRP		0x00000008		/*0010(x)*/
#define THINK_MUTEX_ROTH		0x00000004		/*0004(r)*/
#define THINK_MUTEX_WOTH		0x00000002		/*0002(w)*/
#define THINK_MUTEX_XOTH		0x00000001		/*0001(x)*/

#define THINK_MUTEX_MASK_SECURITY	0x000001FF	/*0777(rwxrwxrwx)*/

struct __think_mutex{
#ifdef __THINK_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int semid;
#endif
	int flags;
};
typedef struct __think_mutex THINK_MUTEX;

THINK_MUTEX *think_mutexopen(const char *name,int flags);
int think_mutexlock(THINK_MUTEX *mutex,int timeout);
int think_mutexunlock(THINK_MUTEX *mutex);
int think_mutexclose(THINK_MUTEX *mutex);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_MUTEX_H__ */
