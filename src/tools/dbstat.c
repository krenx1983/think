#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbstat dbfile";
	THINK_DB *db;
	THINK_DB_FIELD *p;
	int i;

	if(think_getopts(argc,argv,NULL,0)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	if((db=think_db_openx(argv[optind],THINK_DB_RDONLY,""))==NULL){
		think_showerror();
		exit(-1);
	}

	/* summary info */
	printf("type=[%d]\n",db->type);
	printf("version=[%d]\n",db->version);
	printf("field_count=[%d]\n",db->field_count);
	printf("created_time=[%s]\n",db->created_time);
	printf("last_modified_time=[%s]\n",db->last_modified_time);
	printf("record_count=[%d]\n",db->record_count);
	printf("transaction_flag=[%d]\n",db->transaction_flag);
	printf("option_length=[%d]\n",db->option_length);

	/* field info */
	puts("------------------------------------");
	printf("%-12s|%-7s|%-7s|%s\n","Name","Type","Length","Flags");
	puts("------------------------------------");
	for(i=0,p=db->field_array;i<db->field_count;i++,p++){
		printf("%-12s|%-7c|%-7d|%s\n",p->name,p->type,p->length,"");
	}

	return 0;
}
