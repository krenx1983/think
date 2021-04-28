#include "think.h"
#include "think_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfload [-d delimiter] dbffile txtfile";
	THINK_DBF *dbf;
	FILE *fp;
	int i,n;
	char d='|',value[256],linebuf[1024];
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
	if((dbf=think_dbf_open(argv[optind],THINK_DBF_WRONLY|THINK_DBF_APPEND,""))==NULL){
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
		if(think_chrstat(linebuf,d)!=dbf->field_count-1){
			printf("field count not match.[recordno=%d]\n",n);
			fclose(fp);
			think_dbf_close(dbf);
			exit(-1);
		}

		/* init record */
		think_dbf_new(dbf);

		/* put fields */
		for(i=0;i<dbf->field_count;i++){
			/*field value*/
			bzero(value,sizeof(value));
			think_strgetfield(linebuf,d,i,'C',value,sizeof(value),0);
			think_dbf_putfield(dbf,i,'C',value,strlen(value),0);
		}
		if(think_dbf_insert(dbf)<0){
			think_showerror();
			fclose(fp);
			think_dbf_close(dbf);
			exit(-1);
		}
		n++;
	}
	printf("%d records inserted.\n",n);
	fclose(fp);
	think_dbf_close(dbf);

	return 0;
}
