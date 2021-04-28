#include "think_error.h"
#include "think_utility.h"
#include "think_conf.h"

THINK_CONF *think_loadconf(const char *file,char d)
{
	char line[1024],value[THINK_CONF_VALUE_SIZE];
	FILE *fp;
	int r,h=0,v=0;
	THINK_CONF *conf;
	THINK_CONF_HLIST *ph,*firsth,*lasth;
	THINK_CONF_VLIST *pv,*firstv,*lastv;

	if((conf=malloc(sizeof(THINK_CONF)))==NULL){
		think_error(0,"[%s]:malloc error![%d:%s]",__func__,think_errno,think_strerror(think_errno));

		return NULL;
	}
	bzero(conf,sizeof(THINK_CONF));
	conf->d=d;
	conf->hcount=0;
	conf->vcount=0;
	conf->hlist=NULL;
	snprintf(conf->file,THINK_CONF_FILE_SIZE,"%s",file);

	if((fp=fopen(file,"r"))==NULL){
		think_error(0,"[%s]:file open failed.[%d:%s][file=%s]",__func__,think_errno,think_strerror(think_errno),file);
		think_freeconf(conf);
		return NULL;
	}
	while(!feof(fp)){
		if(fgets(line,sizeof(line),fp)==NULL)
			break;
		think_strtrim(line);
		if(line[0]=='#' || strlen(line)==0)
			continue;

		r=think_chrstat(line,d);	/* r will be used later */
		if(conf->hcount!=0 && r!=conf->vcount-1){
			think_error(0,"[%s]:column count not match![file=%s][h=%d]",__func__,file,h);
			fclose(fp);
			think_freeconf(conf);
			return NULL;
		}

		/* put line */
		if((ph=malloc(sizeof(THINK_CONF_HLIST)))==NULL){
			think_error(0,"[%s]:malloc error![%d:%s]",__func__,think_errno,think_strerror(think_errno));
			fclose(fp);
			think_freeconf(conf);
			return NULL;
		}
		ph->vlist=NULL;
		ph->prior=NULL;
		ph->next=NULL;
		if(conf->hlist==NULL){
			ph->prior=ph;
			ph->next=ph;
			conf->hlist=ph;
		}else{
			firsth=conf->hlist;
			lasth=conf->hlist->prior;
			ph->next=firsth;
			ph->prior=lasth;
			firsth->prior=ph;
			lasth->next=ph;
		}

		/* put columns */
		for(v=0;v<=r;v++){
			if(think_strgetfield(line,d,v,'C',value,sizeof(value),0)<0){
				think_error(0,"[%s]:[file=%s][h=%d][v=%d]:%s",__func__,file,h,v,__think_errmsg__);
				fclose(fp);
				think_freeconf(conf);
				return NULL;
			}
			if((pv=malloc(sizeof(THINK_CONF_VLIST)))==NULL){
				think_error(0,"[%s]:malloc error![%d:%s]",__func__,think_errno,think_strerror(think_errno));
				fclose(fp);
				think_freeconf(conf);
				return NULL;
			}
			strcpy(pv->value,value);
			pv->prior=NULL;
			pv->next=NULL;
			if(ph->vlist==NULL){
				pv->prior=pv;
				pv->next=pv;
				ph->vlist=pv;
			}else{
				firstv=ph->vlist;
				lastv=ph->vlist->prior;
				pv->next=firstv;
				pv->prior=lastv;
				firstv->prior=pv;
				lastv->next=pv;
			}
		}
		h++;	/* line plus */
		conf->hcount=h;
		if(conf->hcount==1)
			conf->vcount=r+1;
	}
	fclose(fp);

	return conf;
}
int think_getconf(THINK_CONF *conf,int h,int v,char type,void *value,unsigned int size,int flags)
{
	THINK_CONF_HLIST *ph;
	THINK_CONF_VLIST *pv;
	int i;
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if(h>=conf->hcount || v>=conf->vcount){
		think_error(0,"[%s]:h or v out of range.[h=%d,v=%d]",__func__,h,v);
		return -1;
	}
	ph=conf->hlist;
	for(i=0;i<h;i++)
		ph=ph->next;
	pv=ph->vlist;
	for(i=0;i<v;i++)
		pv=pv->next;
	if(strlen(pv->value)==0 && (flags & THINK_CONF_NOT_NULL)){
		think_error(0,"[%s]:field value is empty.[file=%s][h=%d][v=%d]",__func__,conf->file,h,v);
		return -1;
	}
	switch(type){
		case THINK_CONF_CHAR:
			if(strlen(pv->value)>size){
				think_error(0,"[%s]:value too long![size=%d][len=%d][file=%s][h=%d][v=%d][value=%s]",__func__,size,strlen(pv->value),conf->file,h,v,pv->value);
				return -1;
			}
			if(strlen(pv->value)==size && (!flags & THINK_CONF_NOT_STRING)){
				think_error(0,"[%s]:value equal to size![size=%d][file=%s][h=%d][v=%d][value=%s]",__func__,strlen(pv->value),conf->file,h,v,pv->value);
				return -1;
			}
			strncpy(value,pv->value,size);
			return strlen(pv->value);
		case THINK_CONF_SHORT:
			t_short=atol(pv->value);
			if(t_short==0 && (flags & THINK_CONF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[file=%s][h=%d][v=%d]",__func__,conf->file,h,v);
				return -1;
			}
			memcpy(value,&t_short,2);
			break;
		case THINK_CONF_INT:
			t_int=atol(pv->value);
			if(t_int==0 && (flags & THINK_CONF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[file=%s][h=%d][v=%d]",__func__,conf->file,h,v);
				return -1;
			}
			memcpy(value,&t_int,4);
			break;
		case THINK_CONF_LONG:
			t_long=atoll(pv->value);
			if(t_long==0 && (flags & THINK_CONF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[file=%s][h=%d][v=%d]",__func__,conf->file,h,v);
				return -1;
			}
			memcpy(value,&t_long,8);
			break;
		case THINK_CONF_FLOAT:
			t_float=atof(pv->value);
			if(t_float==0.0 && (flags & THINK_CONF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[file=%s][h=%d][v=%d]",__func__,conf->file,h,v);
				return -1;
			}
			memcpy(value,&t_float,4);
			break;
		case THINK_CONF_DOUBLE:
			t_double=atof(pv->value);
			if(t_double==0.0 && (flags & THINK_CONF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[file=%s][h=%d][v=%d]",__func__,conf->file,h,v);
				return -1;
			}
			memcpy(value,&t_double,8);
			break;
		default:
			think_error(0,"[%s]:illegal field type.[type='%c'][h=%d][v=%d]",__func__,type,h,v);
			return -1;
	}

	return 0;
}
int think_showconf(THINK_CONF *conf)
{
	THINK_CONF_HLIST *ph;
	THINK_CONF_VLIST *pv;
	int i,j;

	if(conf->hcount==0){
		printf("conflist is empty.");
		return 0;
	}
	ph=conf->hlist;
	for(i=0;i<conf->hcount;i++){
		pv=ph->vlist;
		for(j=0;j<conf->vcount;j++){
			if(j==0)
				printf("%s",pv->value);
			else
				printf("|%s",pv->value);
			pv=pv->next;
		}
		printf("\n");
		ph=ph->next;
	}

	return 0;
}
int think_freeconf(THINK_CONF *conf)
{
	THINK_CONF_HLIST *ph;
	THINK_CONF_VLIST *pv;

	while(conf->hlist){
		ph=conf->hlist;
		while(ph->vlist){
			pv=ph->vlist;
			if(pv==pv->next){
				free(pv);
				break;
			}
			pv->next->prior=pv->prior;
			pv->prior->next=pv->next;
			ph->vlist=pv->next;
			free(pv);
		}
		if(ph==ph->next){
			free(ph);
			break;
		}
		ph->next->prior=ph->prior;
		ph->prior->next=ph->next;
		conf->hlist=ph->next;
		free(ph);
	}
	free(conf);

	return 0;
}
