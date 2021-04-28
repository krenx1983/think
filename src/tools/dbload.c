#include "think.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbload [-d delimiter] dbfile txtfile";
	THINK_DB *db;
	FILE *fp;
	int i,n;
	THINK_SHORT ts;
	THINK_INT ti;
	THINK_LONG tl;
	THINK_FLOAT tf;
	THINK_DOUBLE td;
	char d='|',value[1024*1024],linebuf[1024*1024];
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
	if((db=think_db_openx(argv[optind],THINK_DB_WRONLY|THINK_DB_APPEND,""))==NULL){
		think_showerror();
		exit(-1);
	}
	if((fp=fopen(argv[optind+1],"r"))==NULL){
		printf("file open failed![errno=%d][file=%s]",errno,argv[1]);
		exit(-1);
	}
	n=0;
	while(1){
		bzero(linebuf,sizeof(linebuf));
		if(fgets(linebuf,sizeof(linebuf),fp)==NULL)
			break;
		think_strtrim(linebuf);
		if(strlen(linebuf)==0)
			continue;

		/*check fields*/
		if(think_chrstat(linebuf,d)!=db->field_count-1){
			printf("field count not match.[recordno=%d]\n",n);
			fclose(fp);
			think_db_close(db);
			exit(-1);
		}

		/* init record */
		think_db_new(db);

		/* put fields */
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
					think_strgetfield(linebuf,d,i,'C',value,sizeof(value),0);
					think_db_putfield(db,i,value,strlen(value),0);
					break;
				case 'S':
					think_strgetfield(linebuf,d,i,'S',&ts,0,0);
					think_db_putfield(db,i,&ts,0,0);
					break;
				case 'I':
					think_strgetfield(linebuf,d,i,'I',&ti,0,0);
					think_db_putfield(db,i,&ti,0,0);
					break;
				case 'L':
					think_strgetfield(linebuf,d,i,'L',&tl,0,0);
					think_db_putfield(db,i,&tl,0,0);
					break;
				case 'F':
					think_strgetfield(linebuf,d,i,'F',&tf,0,0);
					think_db_putfield(db,i,&tf,0,0);
					break;
				case 'D':
					think_strgetfield(linebuf,d,i,'D',&td,0,0);
					think_db_putfield(db,i,&td,0,0);
					break;
				default:
					break;
			}
		}
		if(think_db_insert(db)<0){
			think_showerror();
			fclose(fp);
			think_db_close(db);
			exit(-1);
		}
		n++;
	}
	printf("%d records inserted.\n",n);
	fclose(fp);
	think_db_close(db);

	return 0;
}
