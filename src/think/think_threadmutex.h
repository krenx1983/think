/*
 * think_threadmutex.h: Think Thread Mutex Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_THREADMUTEX_H__
#define __THINK_THREADMUTEX_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

struct __think_threadmutex{
#ifdef __THINK_WINDOWS__ /* Windows */
		CRITICAL_SECTION cs;
#else /* UNIX */
		pthread_mutex_t *threadmutex;
#endif
};
typedef struct __think_threadmutex THINK_THREADMUTEX;


THINK_THREADMUTEX *think_threadmutex_open();
int think_threadmutex_lock(THINK_THREADMUTEX *threadmutex,int timeout);
int think_threadmutex_unlock(THINK_THREADMUTEX *threadmutex);
int think_threadmutex_close(THINK_THREADMUTEX *threadmutex);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_THREADMUTEX_H__ */
