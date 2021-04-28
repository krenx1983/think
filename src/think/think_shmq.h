/*
 * think_shmq.h: Think Share Memory Queue Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_SHMQ_H__
#define __THINK_SHMQ_H__

#include "think_os.h"
#include "think_shm.h"
#include "think_sem.h"
#include "think_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* open mode */
#define THINK_SHMQ_CREAT		0x80000000
#define THINK_SHMQ_EXCL			0x40000000
#define THINK_SHMQ_TRUNC		0x20000000
#define THINK_SHMQ_RDONLY		0x10000000
#define THINK_SHMQ_RUSR			0x00000100		/*0400(r)*/
#define THINK_SHMQ_WUSR			0x00000080		/*0200(w)*/
#define THINK_SHMQ_XUSR			0x00000040		/*0100(x)*/
#define THINK_SHMQ_RGRP			0x00000020		/*0040(r)*/
#define THINK_SHMQ_WGRP			0x00000010		/*0020(w)*/
#define THINK_SHMQ_XGRP			0x00000008		/*0010(x)*/
#define THINK_SHMQ_ROTH			0x00000004		/*0004(r)*/
#define THINK_SHMQ_WOTH			0x00000002		/*0002(w)*/
#define THINK_SHMQ_XOTH			0x00000001		/*0001(x)*/

#define THINK_SHMQ_MASK_SECURITY	0x000001FF	/*0777(rwxrwxrwx)*/

typedef struct {
	int size;
	int totalsize;
	int freesize;
	int head;
	int tail;
	int msgnum;
}THINK_SHMQHEAD;

typedef struct {
	THINK_SHM *shm;
	THINK_SEM *sem;
	THINK_MUTEX *mutex;
	int flags;
	THINK_SHMQHEAD *head;
	void *data;
}THINK_SHMQ;

THINK_SHMQ *think_shmqopen(const char *name,int size,int flags);
int think_shmqget(THINK_SHMQ *shmq,void *buf,int size,int timeout);
int think_shmqput(THINK_SHMQ *shmq,const void *buf,int len,int timeout);
int think_shmqclose(THINK_SHMQ *shmq);

#ifdef __cplusplus
}
#endif

#endif /* __THINK_SHMQ_H__ */
