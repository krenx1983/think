#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: getcfg [-p cfgpath] [-f field] cfgfile name";
	char cfgpath[256],field[128],value[1024];
	THINK_CFG *cfg;
	THINK_OPTARRAY optarray[2]={
		{'p','C',cfgpath,sizeof(cfgpath),THINK_OPT_ARG,NULL},
		{'f','C',field,sizeof(field),THINK_OPT_ARG,NULL}
	};

	memset(cfgpath,0x00,sizeof(cfgpath));
	memset(field,0x00,sizeof(field));
	if(think_getopts(argc,argv,optarray,2)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=2){
		puts(usage);
		exit(-1);
	}
	if((cfg=think_loadcfg(argv[optind],cfgpath))==NULL){
		think_showerror();
		exit(-1);
	}
	if(think_getcfg(cfg,field,argv[optind+1],'C',value,sizeof(value),0)<0){
		think_showerror();
		exit(-1);
	}
	printf("%s",value);
	think_freecfg(cfg);

	return 0;
}
