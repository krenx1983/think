/*
 * think_thread.h: Think Thread Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_THREAD_H__
#define __THINK_THREAD_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

struct __think_thread{
#ifdef __THINK_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	pthread_t thread;
#endif
};
typedef struct __think_thread THINK_THREAD;
	
#ifdef __THINK_WINDOWS__ /* Windows */
typedef int (WINAPI *THINK_OS_THREADROUTINE)(void *);
int WINAPI think_os_threadroutine(void *);
#else /* UNIX */
typedef void *(*THINK_OS_THREADROUTINE)(void *);
void *think_os_threadroutine(void *);
#endif

/* think thread routine and arg */
typedef void (*THINK_THREADROUTINE)(void *);
struct __think_threadarg{
	THINK_THREADROUTINE routine;
	void *arg;
};
typedef struct __think_threadarg THINK_THREADARG;
	
THINK_THREAD *think_threadcreate(THINK_THREADROUTINE routine,void *arg);
int think_threadexit(int exitcode);
int think_threadcancel(THINK_THREAD *thread);
int think_threadjoin(THINK_THREAD *thread,int *exitcode,int timeout);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_THREAD_H__ */
