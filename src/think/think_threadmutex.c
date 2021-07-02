#include "think_error.h"
#include "think_utility.h"
#include "think_threadmutex.h"

THINK_THREADMUTEX *think_threadmutex_open()
{
	THINK_THREADMUTEX *threadmutex;

	if((threadmutex=malloc(sizeof(THINK_THREADMUTEX)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}
#ifdef __THINK_WINDOWS__ /* Windows */
	InitializeCriticalSection(&threadmutex->cs);
#else /* UNIX */
	if((threadmutex->threadmutex=malloc(sizeof(pthread_mutex_t)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(threadmutex);
		return NULL;
	}
	if(pthread_mutex_init(threadmutex->threadmutex,NULL)<0){
		think_error(0,"[%s]:pthread_mutex_init error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(threadmutex);
		return NULL;
	}
#endif

	return threadmutex;
}
int think_threadmutex_lock(THINK_THREADMUTEX *threadmutex,int timeout)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	EnterCriticalSection(&threadmutex->cs);
#else /* UNIX */
	if(timeout<0){
		if(pthread_mutex_lock(threadmutex->threadmutex)<0){
			think_error(0,"[%s]:pthread_mutex_lock error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
	}else{
		for(;timeout>=0;timeout--){
			if(pthread_mutex_trylock(threadmutex->threadmutex)<0){
				if(think_errno==EBUSY){
					think_os_msleep(1);
					continue;
				}
				think_error(0,"[%s]:pthread_mutex_lock error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				return -1;
			}
			break;
		}
		if(timeout<0)
			return 0;
	}
#endif

	return 1;
}
int think_threadmutex_unlock(THINK_THREADMUTEX *threadmutex)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	LeaveCriticalSection(&threadmutex->cs);
#else /* UNIX */
	if(pthread_mutex_unlock(threadmutex->threadmutex)<0){
		think_error(0,"[%s]:pthread_mutex_unlock error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#endif
	
	return 0;
}
int think_threadmutex_close(THINK_THREADMUTEX *threadmutex)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	DeleteCriticalSection(&threadmutex->cs);
#else /* UNIX */
	pthread_mutex_destroy(threadmutex->threadmutex);
	free(threadmutex->threadmutex);
#endif
	free(threadmutex);

	return 0;
}

