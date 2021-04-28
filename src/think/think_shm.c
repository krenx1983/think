#include "think_error.h"
#include "think_shm.h"

THINK_SHM *think_shmopen(const char *name,int size,int flags)
{
	THINK_SHM *shm;
	int oflags;
#ifdef __THINK_UNIX__ /* UNIX */
	int key;
#endif

	if((shm=malloc(sizeof(THINK_SHM)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}

#ifdef __THINK_WINDOWS__
	if(flags & THINK_SHM_CREAT){
		if(flags & THINK_SHM_RDONLY)
			oflags=PAGE_READONLY;
		else
			oflags=PAGE_READWRITE;
		if((shm->handle=CreateFileMapping(INVALID_HANDLE_VALUE,NULL,oflags,0,size,name))==NULL){
			think_error(0,"[%s]:CreateFileMapping error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			free(shm);
			return NULL;
		}
		if(flags & THINK_SHM_CREAT && flags & THINK_SHM_EXCL){
			if(think_errno==ERROR_ALREADY_EXISTS){
				think_error(0,"[%s]:ShareMemory exists.",__func__);
				CloseHandle(shm->handle);
				free(shm);
				return NULL;
			}
		}
	}else{
		if(flags & THINK_SHM_RDONLY)
			oflags=FILE_MAP_READ;
		else
			oflags=FILE_MAP_WRITE;
		if((shm->handle=OpenFileMapping(oflags,FALSE,name))==NULL){
			think_error(0,"[%s]:OpenFileMapping error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			CloseHandle(shm->handle);
			free(shm);
			return NULL;
		}
	}
	if(flags & THINK_SHM_RDONLY)
		oflags=FILE_MAP_READ;
	else
		oflags=FILE_MAP_WRITE;
	if((shm->addr=MapViewOfFile(shm->handle,oflags,0,0,0))==NULL){
		think_error(0,"[%s]:MapViewOfFile error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		CloseHandle(shm->handle);
		free(shm);
		return NULL;
	}
#else /* UNIX */
	if((key=ftok(name,1))<0){
		think_error(0,"[%s]:ftok error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(shm);
		return NULL;
	}

	oflags=0;
	if(flags & THINK_SHM_CREAT)
		oflags|=IPC_CREAT;
	if(flags & THINK_SHM_EXCL)
		oflags|=IPC_EXCL;
	if(flags & THINK_SHM_RUSR)
		oflags|=S_IRUSR;
	if(flags & THINK_SHM_WUSR)
		oflags|=S_IWUSR;
	if(flags & THINK_SHM_RGRP)
		oflags|=S_IRGRP;
	if(flags & THINK_SHM_WGRP)
		oflags|=S_IWGRP;
	if(flags & THINK_SHM_ROTH)
		oflags|=S_IROTH;
	if(flags & THINK_SHM_WOTH)
		oflags|=S_IWOTH;

	if(flags & THINK_SHM_TRUNC){
		if((shm->shmid=shmget(key,0,0))>0){
			if(shmctl(shm->shmid,IPC_RMID,NULL)<0){
				think_error(0,"[%s]:shmctl error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				free(shm);
				return NULL;
			}
		}
	}
	if((shm->shmid=shmget(key,size,oflags))<0){
		think_error(0,"[%s]:shmget error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(shm);
		return NULL;
	}
	oflags=0;
	if(flags & THINK_SHM_RDONLY)
		oflags|=SHM_RDONLY;
	if((shm->addr=shmat(shm->shmid,NULL,oflags))==SHM_FAILED){
		think_error(0,"[%s]:shmat error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(shm);
		return NULL;
	}
#endif

	return shm;
}
int think_shmclose(THINK_SHM *shm)
{
#ifdef __THINK_WINDOWS__ /* Windows */
	UnmapViewOfFile(shm->addr);
	CloseHandle(shm->handle);
#else /* UNIX */
	shmdt(shm->addr);
#endif
	free(shm);

	return 0;
}
