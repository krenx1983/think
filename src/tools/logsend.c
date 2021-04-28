#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: logsend [-GTELP] [-a addr] [-n name] [-l level] [-d desc] info";
	THINK_LOG *lp;
	char addr[256],name[256],desc[32],info[1024];
	int G_Exists,T_Exists,E_Exists,L_Exists,P_Exists;
	int flags,level;
	THINK_OPTARRAY optarray[9]={
		{'G','C',NULL,0,0,&G_Exists},
		{'T','C',NULL,0,0,&T_Exists},
		{'E','C',NULL,0,0,&E_Exists},
		{'L','C',NULL,0,0,&L_Exists},
		{'P','C',NULL,0,0,&P_Exists},
		{'a','C',addr,sizeof(addr),THINK_OPT_ARG,NULL},
		{'n','C',name,sizeof(name),THINK_OPT_ARG,NULL},
		{'l','I',&level,0,THINK_OPT_ARG,NULL},
		{'d','C',desc,sizeof(desc),THINK_OPT_ARG,NULL}
	};
	THINK_ENVARRAY envarray[2]={
		{"LOG_ADDR",'C',addr,sizeof(addr),0,NULL},
		{"LOG_NAME",'C',name,sizeof(name),0,NULL}
	};

	bzero(addr,sizeof(addr));
	bzero(name,sizeof(name));
	level=0;
	bzero(desc,sizeof(desc));
	if(think_getenvs(envarray,2)<0){
		think_showerror();
		exit(-1);
	}
	if(think_getopts(argc,argv,optarray,9)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	flags=0;
	if(G_Exists)
		flags|=THINK_LOG_G;
	if(T_Exists)
		flags|=THINK_LOG_TRUNC;
	if(E_Exists)
		flags|=THINK_LOG_EXCL;
	if(L_Exists)
		flags|=THINK_LOG_LOCAL;
	flags|=THINK_LOG_DEBUG;
	/* net start */
	think_netstart();

	if((lp=think_logopen(addr,name,flags))==NULL){
		think_showerror();
		exit(-1);
	}
	flags=0;
	if(P_Exists)
		flags|=THINK_LOG_PRINT;
	flags|=level;
	snprintf(info,sizeof(info),"%s",argv[optind]);
	if(think_logsend(lp,flags,desc,info)<0){
		think_showerror();
		think_logclose(lp);
		exit(-1);
	}
	think_logclose(lp);

	/* net close */
	think_netstop();

	return 0;
}
