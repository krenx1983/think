#include "think_error.h"
#include "think_utility.h"
#include "think_sem.h"

THINK_SEM *think_semopen(const char *name,int flags)
{
	THINK_SEM *sem;
#ifdef __THINK_UNIX__
	int key,oflags=0;
	THINK_SEMUN su;
	char tmpname[100];
#endif

	if((sem=malloc(sizeof(THINK_SEM)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}
	sem->flags=flags;
#ifdef __THINK_WINDOWS__
	if((sem->handle=CreateSemaphore(NULL,0,65536,name))==NULL){
		think_error(0,"[%s]:CreateSemaphore error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(sem);
		return NULL;
	}
	if(flags & THINK_SEM_CREAT && flags & THINK_SEM_EXCL){
		if(think_errno==ERROR_ALREADY_EXISTS){
			think_error(0,"[%s]:Semaphore exists.",__func__);
			CloseHandle(sem->handle);
			free(sem);
			return NULL;
		}
	}
#else
	if(!name){
		sprintf(tmpname,"/proc/%d",getpid());
		name=tmpname;
	}
	if((key=ftok(name,1))<0){
		think_error(0,"[%s]:ftok error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(sem);
		return NULL;
	}

	if(flags & THINK_SEM_CREAT)
		oflags|=IPC_CREAT;
	if(flags & THINK_SEM_EXCL)
		oflags|=IPC_EXCL;
	if(flags & THINK_SEM_RUSR)
		oflags|=S_IRUSR;
	if(flags & THINK_SEM_WUSR)
		oflags|=S_IWUSR;
	if(flags & THINK_SEM_RGRP)
		oflags|=S_IRGRP;
	if(flags & THINK_SEM_WGRP)
		oflags|=S_IWGRP;
	if(flags & THINK_SEM_ROTH)
		oflags|=S_IROTH;
	if(flags & THINK_SEM_WOTH)
		oflags|=S_IWOTH;

	if((sem->semid=semget(key,1,oflags))<0){
		think_error(0,"[%s]:semget error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(sem);
		return NULL;
	}
	if(flags & THINK_SEM_TRUNC){
		su.val=0;
		if(semctl(sem->semid,0,SETVAL,su)<0){
			think_error(0,"[%s]:semctl error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			free(sem);
			return NULL;
		}
	}
#endif

	return sem;
}

int think_semget(THINK_SEM *sem,int timeout)
{
	int r;
#ifdef __THINK_UNIX__
	struct sembuf sb;
#endif

#ifdef __THINK_WINDOWS__ /* Windows */
	if(timeout<0)
		timeout=INFINITE;
	if((r=WaitForSingleObject(sem->handle,timeout))!=WAIT_OBJECT_0){
		if(r==WAIT_TIMEOUT)
			return 0;
		think_error(0,"[%s]:WaitForSingleObject error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#else /* UNIX */
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=0;
	if(sem->flags & THINK_SEM_UNDO)
		sb.sem_flg|=SEM_UNDO;
	if(timeout<0){
		if(semop(sem->semid,&sb,1)<0){
			if(think_errno==EINTR)
				return 0;
			think_error(0,"[%s]:semop error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
	}else if(timeout==0){
		sb.sem_flg|=IPC_NOWAIT;
		if(semop(sem->semid,&sb,1)<0){
			if(think_errno==EINTR || think_errno==EAGAIN)
				return 0;
			think_error(0,"[%s]:semop error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
	}else{
		sb.sem_flg|=IPC_NOWAIT;
		for(;timeout>=0;think_os_msleep(1),timeout--){
			if(semop(sem->semid,&sb,1)<0){
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
int think_semput(THINK_SEM *sem)
{
#ifdef __THINK_UNIX__
	struct sembuf sb;
#endif

#ifdef __THINK_WINDOWS__ /* Windows */
	if(ReleaseSemaphore(sem->handle,1,NULL)==0){
		think_error(0,"[%s]:ReleaseSemaphore error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#else /* UNIX */
	sb.sem_num=0;
	sb.sem_op=1;
	sb.sem_flg=0;
	if(sem->flags & THINK_SEM_UNDO)
		sb.sem_flg|=SEM_UNDO;
	if(semop(sem->semid,&sb,1)<0){
		think_error(0,"[%s]:semop error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
#endif

	return 0;
}
int think_semclose(THINK_SEM *sem)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	CloseHandle(sem->handle);
#else /* UNIX */
#endif
	free(sem);

	return 0;
}

