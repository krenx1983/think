#include "think_error.h"
#include "think_thread.h"

#ifdef __THINK_WINDOWS__ /* Windows */
int WINAPI think_os_threadroutine(void *arg)
{
	THINK_THREADARG *threadarg;

	threadarg=arg;
	threadarg->routine(threadarg->arg);
	free(arg);

	return 0;
}
#else /* UNIX */
void *think_os_threadroutine(void *arg)
{
	THINK_THREADARG *threadarg;

	threadarg=arg;
	threadarg->routine(threadarg->arg);
	free(arg);

	return NULL;
}
#endif

THINK_THREAD *think_threadcreate(THINK_THREADROUTINE routine,void *arg)
{
	THINK_THREAD *thread;
	THINK_THREADARG *threadarg;

	if((thread=malloc(sizeof(THINK_THREAD)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}

	if((threadarg=malloc(sizeof(THINK_THREADARG)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(thread);
		return NULL;
	}
	threadarg->routine=routine;
	threadarg->arg=arg;
#ifdef __THINK_WINDOWS__ /* Windows */
	if((thread->handle=CreateThread(NULL,0,think_os_threadroutine,threadarg,0,NULL))==NULL){
		think_error(0,"[%s]:CreateThread error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(threadarg);
		free(thread);
		return NULL;
	}
#else /* UNIX */
	if(pthread_create(&thread->thread,NULL,think_os_threadroutine,threadarg)<0){
		think_error(0,"[%s]:pthread_create error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(threadarg);
		free(thread);
		return NULL;
	}
#endif

	return thread;
}
int think_threadexit(int exitcode)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	ExitThread(exitcode);
#else /* UNIX */
	int *p=&exitcode;
	pthread_exit(p);
#endif

	return 0;
}
int think_threadcancel(THINK_THREAD *thread)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	if(TerminateThread(thread->handle,0)==0){
		think_error(0,"[%s]:TerminateThread error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#else /* UNIX */
	if(pthread_cancel(thread->thread)<0){
		think_error(0,"[%s]:pthread_cancel error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#endif
	free(thread);

	return 0;
}
int think_threadjoin(THINK_THREAD *thread,int *exitcode,int timeout)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	int r;

	if(timeout<0)
		timeout=INFINITE;
	if((r=WaitForSingleObject(thread->handle,timeout))!=WAIT_OBJECT_0){
		if(r==WAIT_TIMEOUT)
			return 0;
		think_error(0,"[%s]:WaitForSingleObject error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
	if(exitcode)
		GetExitCodeThread(thread->handle,exitcode);
#else /* UNIX */
	int *p;
	if(pthread_join(thread->thread,(void **)&p)<0){
		think_error(0,"[%s]:pthread_join error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
	if(exitcode)
		*exitcode=*p;
#endif
	free(thread);

	return 1;
}
