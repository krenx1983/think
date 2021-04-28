#include "think.h"
#include "think_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfcreate dbffile conffile";
	THINK_DBF *dbf;
	int flags;

	if(think_getopts(argc,argv,NULL,0)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=2){
		puts(usage);
		exit(-1);
	}
	flags=THINK_DBF_CREAT|THINK_DBF_TRUNC;
	if((dbf=think_dbf_open(argv[optind],flags,argv[optind+1]))==NULL){
		think_showerror();
		exit(-1);
	}
	think_dbf_close(dbf);

	return 0;
}
