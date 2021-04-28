#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbinsert [-d delimiter] dbfile record";
	THINK_DB *db;
	int i;
	THINK_SHORT ts;
	THINK_INT ti;
	THINK_LONG tl;
	THINK_FLOAT tf;
	THINK_DOUBLE td;
	char d='|',value[1024];
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

	if((db=think_db_openx(argv[optind],THINK_DB_WRONLY|THINK_DB_APPEND|THINK_DB_SYNC,""))==NULL){
		think_showerror();
		exit(-1);
	}
	/*check fields*/
	if(think_chrstat(argv[optind+1],d)!=db->field_count-1){
		printf("field num not match.\n");
		think_db_close(db);
		exit(-1);
	}
	think_db_new(db);
	/*init record*/
	for(i=0;i<db->field_count;i++){
		/*field value*/
		bzero(value,sizeof(value));
		ts=0;
		ti=0;
		tl=0;
		tf=0.0;
		td=0.0;
		switch(db->field_array[i].type){
			case 'C':
				think_strgetfield(argv[optind+1],d,i,'C',value,sizeof(value),0);
				think_db_putfield(db,i,value,strlen(value),0);
				break;
			case 'S':
				think_strgetfield(argv[optind+1],d,i,'S',&ts,0,0);
				think_db_putfield(db,i,&ts,0,0);
				break;
			case 'I':
				think_strgetfield(argv[optind+1],d,i,'I',&ti,0,0);
				think_db_putfield(db,i,&ti,0,0);
				break;
			case 'L':
				think_strgetfield(argv[optind+1],d,i,'L',&tl,0,0);
				think_db_putfield(db,i,&tl,0,0);
				break;
			case 'F':
				think_strgetfield(argv[optind+1],d,i,'F',&tf,0,0);
				think_db_putfield(db,i,&tf,0,0);
				break;
			case 'D':
				think_strgetfield(argv[optind+1],d,i,'D',&td,0,0);
				think_db_putfield(db,i,&td,0,0);
				break;
			default:
				break;
		}
	}
	if(think_db_insert(db)<0){
		think_showerror();
		think_db_close(db);
		exit(-1);
	}
	think_db_close(db);

	return 0;
}
