#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: showconf [-d delimiter] conffile";
	THINK_CONF *conf;
	char d='|';
	THINK_OPTARRAY optarray[1]={
		{'d','C',&d,1,THINK_OPT_NOT_NULL|THINK_OPT_NOT_STRING|THINK_OPT_ARG,NULL}
	};

	if(think_getopts(argc,argv,optarray,1)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	if((conf=think_loadconf(argv[optind],d))==NULL){
		think_showerror();
		exit(-1);
	}
	think_showconf(conf);
	think_freeconf(conf);

	return 0;
}
