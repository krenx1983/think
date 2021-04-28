#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbshow dbfile";
	THINK_DB *db;
	int i;
	char value[1024];
	THINK_SHORT ts;
	THINK_INT ti;
	THINK_LONG tl;
	THINK_FLOAT tf;
	THINK_DOUBLE td;

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
	/* show title */
	for(i=0;i<db->field_count;i++){
		if(i==0){
			if((db->field_array+i)->type=='C'){
				printf("%-*s",(db->field_array+i)->length
				>strlen((db->field_array+i)->name)
				?(db->field_array+i)->length
				:strlen((db->field_array+i)->name)
				,(db->field_array+i)->name);
			}else{
				printf("%*s",(db->field_array+i)->length
				>strlen((db->field_array+i)->name)
				?(db->field_array+i)->length
				:strlen((db->field_array+i)->name)
				,(db->field_array+i)->name);
			}
		}else{
			if((db->field_array+i)->type=='C'){
				printf("|%-*s",(db->field_array+i)->length
				>strlen((db->field_array+i)->name)
				?(db->field_array+i)->length
				:strlen((db->field_array+i)->name)
				,(db->field_array+i)->name);
			}else{
				printf("|%*s",(db->field_array+i)->length
				>strlen((db->field_array+i)->name)
				?(db->field_array+i)->length
				:strlen((db->field_array+i)->name)
				,(db->field_array+i)->name);
			}
		}
	}
	printf("\n");
	while(think_db_fetch(db)==1){
		for(i=0;i<db->field_count;i++){
			switch((db->field_array+i)->type){
				case 'C':
					think_db_getfield(db,i,value,sizeof(value),0);
					break;
				case 'S':
					think_db_getfield(db,i,&ts,0,0);
					sprintf(value,"%d",ts);
					break;
				case 'I':
					think_db_getfield(db,i,&ti,0,0);
					sprintf(value,"%d",ti);
					break;
				case 'L':
					think_db_getfield(db,i,&tl,0,0);
					sprintf(value,"%d",tl);
					break;
				case 'F':
					think_db_getfield(db,i,&tf,0,0);
					sprintf(value,"%f",tf);
					break;
				case 'D':
					think_db_getfield(db,i,&td,0,0);
					sprintf(value,"%f",td);
					break;
				default:
					break;
			}
			if(i==0){
				if((db->field_array+i)->type=='C'){
					printf("%-*s",(db->field_array+i)->length
					>strlen((db->field_array+i)->name)
					?(db->field_array+i)->length
					:strlen((db->field_array+i)->name)
					,value);
				}else{
					printf("%*s",(db->field_array+i)->length
					>strlen((db->field_array+i)->name)
					?(db->field_array+i)->length
					:strlen((db->field_array+i)->name)
					,value);
				}
			}else{
				if((db->field_array+i)->type=='C'){
					printf("|%-*s",(db->field_array+i)->length
					>strlen((db->field_array+i)->name)
					?(db->field_array+i)->length
					:strlen((db->field_array+i)->name)
					,value);
				}else{
					printf("|%*s",(db->field_array+i)->length
					>strlen((db->field_array+i)->name)
					?(db->field_array+i)->length
					:strlen((db->field_array+i)->name)
					,value);
				}
			}
		}
		printf("\n");
	}
	printf("record count: %d\n",db->record_count);
	think_db_close(db);

	return 0;
}
