#include "think_error.h"
#include "think_utility.h"
#include "think_shmq.h"

THINK_SHMQ *think_shmqopen(const char *name,int size,int flags)
{
	THINK_SHMQ *shmq;
	char shmname[128],semname[128],mutexname[128];
	int oflags;
#ifdef __THINK_UNIX__
	int key,shmqexist;
#endif

	sprintf(shmname,"%s_shm",name);
	sprintf(semname,"%s_sem",name);
	sprintf(mutexname,"%s_mutex",name);

	/* check if queue exists on unix */
#ifdef __THINK_UNIX__
	if(flags & THINK_SHMQ_CREAT){
		if((key=ftok(shmname,1))<0){
			think_error(0,"[%s]:ftok error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return NULL;
		}
		if(shmget(key,0,0)>0){
			shmqexist=1;
		}else{
			shmqexist=0;
		}
	}
#endif

	if((shmq=malloc(sizeof(THINK_SHMQ)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}

	oflags=0;
	if(flags & THINK_SHMQ_CREAT)
		oflags|=THINK_SHM_CREAT;
	if(flags & THINK_SHMQ_EXCL)
		oflags|=THINK_SHM_EXCL;
	if(flags & THINK_SHMQ_TRUNC)
		oflags|=THINK_SHM_TRUNC;
	oflags|=flags & THINK_SHMQ_MASK_SECURITY;

	if((shmq->shm=think_shmopen(shmname,size,oflags))==NULL){
		think_errorerror();
		free(shmq);
		return NULL;
	}

	oflags=0;
	if(flags & THINK_SHMQ_CREAT)
		oflags|=THINK_SEM_CREAT;
	if(flags & THINK_SHMQ_EXCL)
		oflags|=THINK_SEM_EXCL;
	if(flags & THINK_SHMQ_TRUNC)
		oflags|=THINK_SEM_TRUNC;
	oflags|=flags & THINK_SHMQ_MASK_SECURITY;

	if((shmq->sem=think_semopen(semname,oflags))==NULL){
		think_errorerror();
		think_shmclose(shmq->shm);
		free(shmq);
		return NULL;
	}

	oflags=0;
	if(flags & THINK_SHMQ_CREAT)
		oflags|=THINK_MUTEX_CREAT;
	if(flags & THINK_SHMQ_EXCL)
		oflags|=THINK_MUTEX_EXCL;
	if(flags & THINK_SHMQ_TRUNC)
		oflags|=THINK_MUTEX_TRUNC;
	oflags|=flags & THINK_SHMQ_MASK_SECURITY;

	if((shmq->mutex=think_mutexopen(mutexname,oflags))==NULL){
		think_errorerror();
		think_shmclose(shmq->shm);
		think_semclose(shmq->sem);
		free(shmq);
		return NULL;
	}
	shmq->flags=flags;
	shmq->head=shmq->shm->addr;
	shmq->data=(char *)shmq->head+sizeof(THINK_SHMQHEAD);

#ifdef __THINK_WINDOWS__
	if(flags & THINK_SHMQ_CREAT){
		shmq->head->size=size;
		shmq->head->totalsize=shmq->head->size-sizeof(THINK_SHMQHEAD);
		shmq->head->freesize=shmq->head->totalsize;
		shmq->head->head=0;
		shmq->head->tail=0;
		shmq->head->msgnum=0;
	}
#else
	if((flags & THINK_SHMQ_TRUNC) || ((flags & THINK_SHMQ_CREAT) && !shmqexist))
		shmq->head->size=size;
	if(((flags & THINK_SHMQ_CREAT) && !shmqexist) || (flags & THINK_SHMQ_TRUNC)){
		shmq->head->totalsize=shmq->head->size-sizeof(THINK_SHMQHEAD);
		shmq->head->freesize=shmq->head->totalsize;
		shmq->head->head=0;
		shmq->head->tail=0;
		shmq->head->msgnum=0;
	}
#endif

	return shmq;
}
int think_shmqget(THINK_SHMQ *shmq,void *buf,int size,int timeout)
{
	THINK_SHMQHEAD *head;
	void *data;
	int r,len;

	if((r=think_semget(shmq->sem,timeout))<0){
		think_errorerror();
		return -1;
	}
	if(r==0)
		return 0;
	if(think_mutexlock(shmq->mutex,-1)<0){
		think_errorerror();
		return -1;
	}
	head=shmq->head;
	data=shmq->data;
	if(head->msgnum==0){
		think_error(0,"[%s]:semaphore ok but no message.",__func__);
		return -1;
	}
	if(head->totalsize-head->head<sizeof(int)){
		memcpy(&len,(char *)data+head->head,head->totalsize-head->head);
		memcpy((char *)&len+(head->totalsize-head->head),data,sizeof(int)-(head->totalsize-head->head));
		if(size<len){
			think_error(0,"[%s]:bufsize too short.[size=%d,msglen=%d]",__func__,size,len);
			return -1;
		}
		head->head=sizeof(int)-(head->totalsize-head->head);
	}else{
		memcpy(&len,(char *)data+head->head,sizeof(int));
		if(size<len){
			think_error(0,"[%s]:bufsize too short.[size=%d,msglen=%d]",__func__,size,len);
			return -1;
		}
		head->head+=sizeof(int);
		if(head->head==head->totalsize)
			head->head=0;
	}
	if(head->totalsize-head->head<len){
		memcpy(buf,(char *)data+head->head,head->totalsize-head->head);
		memcpy((char *)buf+(head->totalsize-head->head),data,len-(head->totalsize-head->head));
		head->head=len-(head->totalsize-head->head);
	}else{
		memcpy(buf,(char *)data+head->head,len);
		head->head+=len;
		if(head->head==head->totalsize)
			head->head=0;
	}
	head->msgnum--;
	head->freesize+=sizeof(int)+len;
	if(think_mutexunlock(shmq->mutex)<0){
		think_errorerror();
		return -1;
	}

	return len;
}
int think_shmqput(THINK_SHMQ *shmq,const void *buf,int len,int timeout)
{
	THINK_SHMQHEAD *head;
	void *data;
	int r;

	if(shmq->flags & THINK_SHMQ_RDONLY){
		think_error(0,"[%s]:shmq is read only.",__func__);
		return -1;
	}
	if(shmq->head->totalsize<len+sizeof(int)){
		think_error(0,"[%s]:buf too long![totalsize=%d][msglen=%d]",__func__,shmq->head->totalsize,len);
		return -1;
	}
	if(timeout<0){
		while(1){
			if((r=think_mutexlock(shmq->mutex,-1))<0){
				think_errorerror();
				return -1;
			}
			if(r==0)
				continue;
			if(shmq->head->freesize>=len+sizeof(int))
				break;
			if(think_mutexunlock(shmq->mutex)<0){
				think_errorerror();
				return -1;
			}
			think_os_msleep(1);
		}
	}else if(timeout==0){
		if((r=think_mutexlock(shmq->mutex,0))<0){
			think_errorerror();
			return -1;
		}
		if(r==0)
			return 0;
		if(shmq->head->freesize<len+sizeof(int)){
			if(think_mutexunlock(shmq->mutex)<0){
				think_errorerror();
				return -1;
			}
			return 0;
		}
	}else{
		for(;timeout>=0;think_os_msleep(1),timeout--){
			if((r=think_mutexlock(shmq->mutex,0))<0){
				think_errorerror();
				return -1;
			}
			if(r==0){
				if(timeout==0)
					return 0;
				continue;
			}
			if(shmq->head->freesize>=len+sizeof(int))
				break;
			if(think_mutexunlock(shmq->mutex)<0){
				think_errorerror();
				return -1;
			}
			if(timeout==0)
				return 0;
		}
	}
	head=shmq->head;
	data=shmq->data;
	if(head->totalsize-head->tail<sizeof(int)+len){
		if(head->totalsize-head->tail<sizeof(int)){
			memcpy((char *)data+head->tail,&len,head->totalsize-head->tail);
			memcpy(data,(char *)&len+(head->totalsize-head->tail),sizeof(int)-(head->totalsize-head->tail));
			memcpy((char *)data+(sizeof(int)-(head->totalsize-head->tail)),buf,len);
			head->tail=sizeof(int)-(head->totalsize-head->tail)+len;
		}else{
			memcpy((char *)data+head->tail,&len,sizeof(int));
			head->tail+=sizeof(int);
			memcpy((char *)data+head->tail,buf,head->totalsize-head->tail);
			memcpy(data,(char *)buf+(head->totalsize-head->tail),len-(head->totalsize-head->tail));
			head->tail=len-(head->totalsize-head->tail);
		}
	}else{
		memcpy((char *)data+head->tail,&len,sizeof(int));
		head->tail+=sizeof(int);
		memcpy((char *)data+head->tail,buf,len);
		head->tail+=len;
		if(head->tail==head->totalsize)
			head->tail=0;
	}
	head->freesize-=sizeof(int)+len;
	head->msgnum++;
	if(think_semput(shmq->sem)<0){
		think_errorerror();
		return -1;
	}
	if(think_mutexunlock(shmq->mutex)<0){
		think_errorerror();
		return -1;
	}

	return len;
}
int think_shmqclose(THINK_SHMQ *shmq)
{
	think_semclose(shmq->sem);
	think_mutexclose(shmq->mutex);
	think_shmclose(shmq->shm);
	free(shmq);

	return 0;
}
