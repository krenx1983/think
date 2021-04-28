#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbcreate dbfile conffile";
	THINK_DB *db;
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
	flags=THINK_DB_CREAT|THINK_DB_TRUNC;
	if((db=think_db_openx(argv[optind],flags,argv[optind+1]))==NULL){
		think_showerror();
		exit(-1);
	}
	think_db_close(db);

	return 0;
}
