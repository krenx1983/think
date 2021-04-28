#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: shmqput [-n name] message";
	THINK_SHMQ *shmq;
	int flags;
	char shmqname[128];
	THINK_OPTARRAY optarray[1]={
		{'n','C',shmqname,sizeof(shmqname),THINK_OPT_ARG|THINK_OPT_NOT_NULL,NULL}
	};
	THINK_ENVARRAY envarray[1]={
		{"SHMQ_NAME",'C',shmqname,sizeof(shmqname),THINK_ENV_NOT_NULL,NULL}
	};

	if(think_getenvs(envarray,1)<0){
		think_showerror();
		return -1;
	}
	if(think_getopts(argc,argv,optarray,1)<0){
		think_showerror();
		puts(usage);
		return -1;
	}
	if(argc-optind!=1){
		puts(usage);
		return -1;
	}
	flags=0777;
	if((shmq=think_shmqopen(shmqname,0,flags))==NULL){
		think_showerror();
		return -1;
	}
	if(think_shmqput(shmq,argv[optind],strlen(argv[optind]),-1)<0){
		think_showerror();
		return -1;
	}
	think_shmqclose(shmq);

	return 0;
}
