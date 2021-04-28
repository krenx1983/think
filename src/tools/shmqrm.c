#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: shmqrm name";
	int key,shmid,semid,mutexid;
	char shmname[128],semname[128],mutexname[128];

	if(think_getopts(argc,argv,NULL,0)<0){
		think_showerror();
		puts(usage);
		return -1;
	}
	if(argc-optind!=1){
		puts(usage);
		return -1;
	}
	sprintf(shmname,"%s_shm",argv[optind]);
	sprintf(semname,"%s_sem",argv[optind]);
	sprintf(mutexname,"%s_mutex",argv[optind]);

	/* shm */
	if((key=ftok(shmname,1))<0){
		printf("ftok error.[%d:%s]\n",think_errno,think_strerror(think_errno));
		return -1;
	}
	if((shmid=shmget(key,0,0))>0){
		if(shmctl(shmid,IPC_RMID,NULL)<0){
			printf("shmctl error.[%d:%s]\n",think_errno,think_strerror(think_errno));
			return -1;
		}
	}

	/* sem */
	if((key=ftok(semname,1))<0){
		printf("ftok error.[%d:%s]\n",think_errno,think_strerror(think_errno));
		return -1;
	}
	if((semid=semget(key,0,0))>0){
		if(semctl(semid,0,IPC_RMID)<0){
			printf("semctl error.[%d:%s]\n",think_errno,think_strerror(think_errno));
			return -1;
		}
	}

	/* mutex */
	if((key=ftok(mutexname,1))<0){
		printf("ftok error.[%d:%s]\n",think_errno,think_strerror(think_errno));
		return -1;
	}
	if((mutexid=semget(key,0,0))>0){
		if(semctl(mutexid,0,IPC_RMID)<0){
			printf("semctl error.[%d:%s]\n",think_errno,think_strerror(think_errno));
			return -1;
		}
	}

	return 0;
}
