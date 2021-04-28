#include "think_error.h"
#include "think_utility.h"
#include "think_sem.h"
#include "think_mutex.h"

THINK_MUTEX *think_mutexopen(const char *name,int flags)
{
	THINK_MUTEX *mutex;
#ifdef __THINK_UNIX__
	int key,semexist,oflags=0;
	THINK_SEMUN su;
#endif

	if((mutex=malloc(sizeof(THINK_MUTEX)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}
	mutex->flags=flags;
#ifdef __THINK_WINDOWS__
	if((mutex->handle=CreateSemaphore(NULL,1,65536,name))==NULL){
		think_error(0,"[%s]:CreateSemaphore error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(mutex);
		return NULL;
	}
	if(flags & THINK_MUTEX_CREAT && flags & THINK_MUTEX_EXCL){
		if(think_errno==ERROR_ALREADY_EXISTS){
			think_error(0,"[%s]:mutex exists.",__func__);
			CloseHandle(mutex->handle);
			free(mutex);
			return NULL;
		}
	}
#else
	if((key=ftok(name,1))<0){
		think_error(0,"[%s]:ftok error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(mutex);
		return NULL;
	}

	if(flags & THINK_MUTEX_CREAT)
		oflags|=IPC_CREAT;
	if(flags & THINK_MUTEX_EXCL)
		oflags|=IPC_EXCL;
	if(flags & THINK_MUTEX_RUSR)
		oflags|=S_IRUSR;
	if(flags & THINK_MUTEX_WUSR)
		oflags|=S_IWUSR;
	if(flags & THINK_MUTEX_RGRP)
		oflags|=S_IRGRP;
	if(flags & THINK_MUTEX_WGRP)
		oflags|=S_IWGRP;
	if(flags & THINK_MUTEX_ROTH)
		oflags|=S_IROTH;
	if(flags & THINK_MUTEX_WOTH)
		oflags|=S_IWOTH;

	if(flags & THINK_MUTEX_CREAT){
		if(semget(key,1,0)<0)
			semexist=0;
		else
			semexist=1;
	}
	if((mutex->semid=semget(key,1,oflags))<0){
		think_error(0,"[%s]:semget error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(mutex);
		return NULL;
	}
	if(((flags & THINK_MUTEX_CREAT) && !semexist) || (flags & THINK_SEM_TRUNC)){
		su.val=1;
		if(semctl(mutex->semid,0,SETVAL,su)<0){
			think_error(0,"[%s]:semctl error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			free(mutex);
			return NULL;
		}
	}
#endif

	return mutex;
}

int think_mutexlock(THINK_MUTEX *mutex,int timeout)
{
	int r;
#ifdef __THINK_UNIX__
	struct sembuf sb;
#endif

#ifdef __THINK_WINDOWS__ /* Windows */
	if(timeout<0)
		timeout=INFINITE;
	if((r=WaitForSingleObject(mutex->handle,timeout))!=WAIT_OBJECT_0){
		if(r==WAIT_TIMEOUT)
			return 0;
		think_error(0,"[%s]:WaitForSingleObject error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#else /* UNIX */
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=SEM_UNDO;
	if(timeout<0){
		if(semop(mutex->semid,&sb,1)<0){
			if(think_errno==EINTR)
				return 0;
			think_error(0,"[%s]:semop error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
	}else if(timeout==0){
		sb.sem_flg|=IPC_NOWAIT;
		if(semop(mutex->semid,&sb,1)<0){
			if(think_errno==EINTR || think_errno==EAGAIN)
				return 0;
			think_error(0,"[%s]:semop error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
	}else{
		sb.sem_flg|=IPC_NOWAIT;
		for(;timeout>=0;think_os_msleep(1),timeout--){
			if(semop(mutex->semid,&sb,1)<0){
				if(think_errno==EINTR)
					return 0;
				if(think_errno==EAGAIN){
					if(timeout==0)
						return 0;
					continue;
				}
				think_error(0,"[%s]:semop error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				return -1;
			}
			break;
		}
	}
#endif

	return 1;
}
int think_mutexunlock(THINK_MUTEX *mutex)
{
#ifdef __THINK_UNIX__
	struct sembuf sb;
#endif

#ifdef __THINK_WINDOWS__ /* Windows */
	if(ReleaseSemaphore(mutex->handle,1,NULL)==0){
		think_error(0,"[%s]:ReleaseSemaphore error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#else /* UNIX */
	sb.sem_num=0;
	sb.sem_op=1;
	sb.sem_flg=SEM_UNDO;
	if(semop(mutex->semid,&sb,1)<0){
		think_error(0,"[%s]:semop error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#endif

	return 0;
}
int think_mutexclose(THINK_MUTEX *mutex)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	CloseHandle(mutex->handle);
#else /* UNIX */
#endif
	free(mutex);

	return 0;
}

