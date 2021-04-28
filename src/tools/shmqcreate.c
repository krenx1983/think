#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: shmqcreate [-te] name size";
	THINK_SHMQ *shmq;
	int flags,T_Exists,E_Exists;
	THINK_OPTARRAY optarray[2]={
		{'t','C',NULL,0,0,&T_Exists},
		{'e','C',NULL,0,0,&E_Exists}
	};

	if(think_getopts(argc,argv,optarray,2)<0){
		think_showerror();
		puts(usage);
		return -1;
	}
	if(argc-optind!=2){
		puts(usage);
		return -1;
	}
	flags=THINK_SHMQ_CREAT|0777;
	if(T_Exists)
		flags|=THINK_SHMQ_TRUNC;
	if(E_Exists)
		flags|=THINK_SHMQ_EXCL;
	if((shmq=think_shmqopen(argv[optind],atol(argv[optind+1]),flags))==NULL){
		think_showerror();
		return -1;
	}
#ifdef __THINK_WINDOWS__
	getchar();
#else
	think_shmqclose(shmq);
#endif

	return 0;
}
