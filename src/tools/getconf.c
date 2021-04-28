#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: getconf [-d delimiter] conffile h v";
	THINK_CONF *conf;
	char d='|',value[1024];
	THINK_OPTARRAY optarray[1]={
		{'d','C',&d,1,THINK_OPT_NOT_NULL|THINK_OPT_NOT_STRING|THINK_OPT_ARG,NULL}
	};

	if(think_getopts(argc,argv,optarray,1)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=3){
		puts(usage);
		exit(-1);
	}
	if((conf=think_loadconf(argv[optind],d))==NULL){
		think_showerror();
		exit(-1);
	}
	if(think_getconf(conf,atol(argv[optind+1]),atol(argv[optind+2]),'C',value,sizeof(value),0)<0){
		think_showerror();
		exit(-1);
	}
	printf("%s",value);
	think_freeconf(conf);

	return 0;
}
