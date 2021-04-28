#include "think.h"
#include "think_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfshow dbffile";
	THINK_DBF *dbf;
	int i;
	char value[256];

	if(think_getopts(argc,argv,NULL,0)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	if((dbf=think_dbf_open(argv[optind],THINK_DBF_RDONLY,""))==NULL){
		think_showerror();
		exit(-1);
	}
	/* show title */
	for(i=0;i<dbf->field_count;i++){
		if(i==0){
			if((dbf->field_array+i)->type=='N'){
				printf("%-*s",(dbf->field_array+i)->length
				>strlen((dbf->field_array+i)->name)
				?(dbf->field_array+i)->length
				:strlen((dbf->field_array+i)->name)
				,(dbf->field_array+i)->name);
			}else{
				printf("%*s",(dbf->field_array+i)->length
				>strlen((dbf->field_array+i)->name)
				?(dbf->field_array+i)->length
				:strlen((dbf->field_array+i)->name)
				,(dbf->field_array+i)->name);
			}
		}else{
			if((dbf->field_array+i)->type=='N'){
				printf("|%-*s",(dbf->field_array+i)->length
				>strlen((dbf->field_array+i)->name)
				?(dbf->field_array+i)->length
				:strlen((dbf->field_array+i)->name)
				,(dbf->field_array+i)->name);
			}else{
				printf("|%*s",(dbf->field_array+i)->length
				>strlen((dbf->field_array+i)->name)
				?(dbf->field_array+i)->length
				:strlen((dbf->field_array+i)->name)
				,(dbf->field_array+i)->name);
			}
		}
	}
	printf("\n");
	while(think_dbf_fetch(dbf)==1){
		for(i=0;i<dbf->field_count;i++){
			think_dbf_getfield(dbf,i,'C',value,sizeof(value),0);
			if(i==0){
				if((dbf->field_array+i)->type=='N'){
					printf("%-*s",(dbf->field_array+i)->length
					>strlen((dbf->field_array+i)->name)
					?(dbf->field_array+i)->length
					:strlen((dbf->field_array+i)->name)
					,value);
				}else{
					printf("%*s",(dbf->field_array+i)->length
					>strlen((dbf->field_array+i)->name)
					?(dbf->field_array+i)->length
					:strlen((dbf->field_array+i)->name)
					,value);
				}
			}else{
				if((dbf->field_array+i)->type=='N'){
					printf("|%-*s",(dbf->field_array+i)->length
					>strlen((dbf->field_array+i)->name)
					?(dbf->field_array+i)->length
					:strlen((dbf->field_array+i)->name)
					,value);
				}else{
					printf("|%*s",(dbf->field_array+i)->length
					>strlen((dbf->field_array+i)->name)
					?(dbf->field_array+i)->length
					:strlen((dbf->field_array+i)->name)
					,value);
				}
			}
		}
		printf("\n");
	}
	printf("record count: %d\n",dbf->record_count);
	think_dbf_close(dbf);

	return 0;
}
