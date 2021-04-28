#include "think.h"
#include "think_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfinsert [-d delimiter] dbffile record";
	THINK_DBF *dbf;
	int i;
	char d='|',value[256];
	THINK_OPTARRAY optarray[1]={
		{'d','C',&d,1,THINK_OPT_ARG|THINK_OPT_NOT_NULL|THINK_OPT_NOT_STRING,NULL}
	};

	if(think_getopts(argc,argv,optarray,1)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=2){
		puts(usage);
		exit(-1);
	}

	if((dbf=think_dbf_open(argv[optind],THINK_DBF_WRONLY|THINK_DBF_APPEND|THINK_DBF_SYNC,""))==NULL){
		think_showerror();
		exit(-1);
	}
	/*check fields*/
	if(think_chrstat(argv[optind+1],d)!=dbf->field_count-1){
		printf("field num not match.\n");
		think_dbf_close(dbf);
		exit(-1);
	}
	think_dbf_new(dbf);
	/*init record*/
	for(i=0;i<dbf->field_count;i++){
		/*field value*/
		bzero(value,sizeof(value));
		think_strgetfield(argv[optind+1],d,i,'C',value,sizeof(value),0);
		think_dbf_putfield(dbf,i,'C',value,strlen(value),0);
	}
	if(think_dbf_insert(dbf)<0){
		think_showerror();
		think_dbf_close(dbf);
		exit(-1);
	}
	think_dbf_close(dbf);

	return 0;
}
