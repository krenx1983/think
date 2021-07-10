/*
 * think_shm.h: Think Share Memory Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_SHM_H__
#define __THINK_SHM_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_SHM_CREAT		0x80000000
#define THINK_SHM_EXCL		0x40000000
#define THINK_SHM_TRUNC		0x20000000
#define THINK_SHM_RDONLY	0x10000000
#define THINK_SHM_RUSR		0x00000100		/*0400(r)*/
#define THINK_SHM_WUSR		0x00000080		/*0200(w)*/
#define THINK_SHM_XUSR		0x00000040		/*0100(x)*/
#define THINK_SHM_RGRP		0x00000020		/*0040(r)*/
#define THINK_SHM_WGRP		0x00000010		/*0020(w)*/
#define THINK_SHM_XGRP		0x00000008		/*0010(x)*/
#define THINK_SHM_ROTH		0x00000004		/*0004(r)*/
#define THINK_SHM_WOTH		0x00000002		/*0002(w)*/
#define THINK_SHM_XOTH		0x00000001		/*0001(x)*/

#define THINK_SHM_MASK_SECURITY	0x000001FF	/*0777(rwxrwxrwx)*/

#ifndef SHM_FAILED
#define SHM_FAILED (void *)-1L
#endif

struct __think_shm{
#ifdef __THINK_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int shmid;
#endif
	void *addr;
};
typedef struct __think_shm THINK_SHM;
THINK_SHM *think_shmopen(const char *name,int size,int flags);
int think_shmclose(THINK_SHM *shm);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_SHM_H__ */
