#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: showcfg [-p cfgpath] cfgfile";
	THINK_CFG *cfg;
	char cfgpath[256];
	THINK_OPTARRAY optarray[1]={
		{'p','C',cfgpath,sizeof(cfgpath),THINK_OPT_ARG,NULL}
	};

	memset(cfgpath,0x00,sizeof(cfgpath));
	if(think_getopts(argc,argv,optarray,1)<0){
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	if((cfg=think_loadcfg(argv[optind],cfgpath))==NULL){
		think_showerror();
		exit(-1);
	}
	think_showcfg(cfg);
	think_freecfg(cfg);

	return 0;
}
