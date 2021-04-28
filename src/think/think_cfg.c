#include "think_error.h"
#include "think_utility.h"
#include "think_file.h"
#include "think_cfg.h"

int __think_loadcfg(THINK_CFG *cfg,const char *file,const char *path);
int __think_putcfg(THINK_CFG *cfg,const char *field,const char *name,const char *value,const char *file,unsigned int lineno);
THINK_CFG_CFGLIST *__think_findcfg(THINK_CFG *cfg,const char *field,const char *name);

THINK_CFG *think_loadcfg(const char *file,const char *path)
{
	THINK_CFG *cfg;

	if((cfg=(THINK_CFG *)malloc(sizeof(THINK_CFG)))==NULL){
		think_error(0,"[%s]:malloc error![%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}
	memset(cfg,0x00,sizeof(THINK_CFG));
	if(__think_loadcfg(cfg,file,path)<0){	/* recursion */
		think_errorerror();
		think_freecfg(cfg);
		return NULL;
	}

	return cfg;
}
int __think_loadcfg(THINK_CFG *cfg,const char *file,const char *path)
{
	char field[THINK_CFG_FIELD_SIZE],name[THINK_CFG_NAME_SIZE],value[THINK_CFG_VALUE_SIZE];
	char cfgfile[THINK_CFG_FILE_SIZE],cfgpath[THINK_CFG_FILE_SIZE];
	char line[1024];
	FILE *fp=NULL;
	int r,i,icount,includeflag=1;
	unsigned int lineno=0;

	if(*file=='/' || strlen(path)==0){	/* abs path or needn't search */
		if((fp=fopen(file,"r"))==NULL){
			think_error(0,"[%s]:file open failed![%d:%s][file=%s]",__func__,think_errno,think_strerror(think_errno),file);
			return -1;
		}
		snprintf(cfgfile,sizeof(cfgfile),"%s",file);
	}else{
		icount=think_chrstat(path,':');
		for(i=0;i<=icount;i++){
			if((r=think_strgetfield(path,':',i,'C',cfgpath,sizeof(cfgpath),0))<0){	/* r will be used later */
				think_errorerror();
				return -1;
			}
			if(r>0){
				if(strcmp(cfgpath,"/")!=0){
					if(cfgpath[r-1]=='/')
						cfgpath[r-1]='\0';
				}
				if(strcmp(cfgpath,"/")!=0)
					snprintf(cfgfile,sizeof(cfgfile),"%s/%s",cfgpath,file);
				else
					snprintf(cfgfile,sizeof(cfgfile),"/%s",file);
			}else{
				snprintf(cfgfile,sizeof(cfgfile),"%s",file);
			}

			if(access(cfgfile,F_OK)<0){
				continue;
			}
			if((fp=fopen(cfgfile,"r"))==NULL){
				think_error(0,"[%s]:file open failed![%d:%s][file=%s]",__func__,think_errno,think_strerror(think_errno),cfgfile);
				return -1;
			}
			break;
		}
		if(fp==NULL){
			think_error(0,"[%s]:file not found![path=%s]",__func__,path);
			return -1;
		}
	}

	bzero(field,sizeof(field));
	while(!feof(fp)){
		if(fgets(line,sizeof(line),fp)==NULL)
			break;
		lineno++;	/* line no */
		think_strtrim(line);
		if(strlen(line)==0)	/* empty line */
			continue;
		if(line[0]=='#'){	/* include or note */
			line[0]=' ';	/* space to trim */
			if((r=think_chrfind(line,'#',0))>0)	/* rm note */
				line[r]='\0';
			think_strtrim(line);
			if(strncmp(line,"include",7)==0){	/* include */
				if(includeflag==0){
					think_error(0,"[%s]:include must be ahead of cfgs![%s:%d]",__func__,cfgfile,lineno);
					fclose(fp);
					return -1;
				}
				memset(line,' ',7);
				think_strtrim(line);
				if(*line!='<' || think_chrstat(line,'>')!=1 || line[strlen(line)-1]!='>'){
					think_error(0,"[%s]:illegal format![%s:%d]",__func__,cfgfile,lineno);
					fclose(fp);
					return -1;
				}
				line[0]=' ';
				line[strlen(line)-1]=' ';
				think_strtrim(line);	/* include file */
				if(__think_loadcfg(cfg,line,path)<0){	/* recursion load */
					fclose(fp);
					return -1;
				}
				continue;
			}else{	/* note */
				continue;
			}
		}
		if((r=think_chrfind(line,'#',0))>0)	/* rm note */
			line[r]='\0';
		think_strtrim(line);
		if(*line=='['){	/* field */
			if(think_chrstat(line,']')!=1 || line[strlen(line)-1]!=']'){
				think_error(0,"[%s]:illegal format![%s:%d]",__func__,cfgfile,lineno);
				fclose(fp);
				return -1;
			}
			line[0]=' ';
			line[strlen(line)-1]=' ';
			think_strtrim(line);
			if(strlen(line)>=THINK_CFG_FIELD_SIZE){
				think_error(0,"[%s]:field name too long![%s:%d][field=%s]",__func__,cfgfile,lineno,line);
				fclose(fp);
				return -1;
			}
			strcpy(field,line);
			includeflag=0;
			continue;
		}
		/* name = value */
		if(think_chrstat(line,'=')!=1){
			think_error(0,"[%s]:illegal format![%s:%d][field=%s]",__func__,cfgfile,lineno,field);
			fclose(fp);
			return -1;
		}
		r=think_chrfind(line,'=',0);	/* r will be used later */
		/* name */
		bzero(name,sizeof(name));
		line[r]='\0';	/* for trim name */
		think_strtrim(line);
		if(strlen(line)>=THINK_CFG_NAME_SIZE){
			think_error(0,"[%s]:name too long![%s:%d][field=%s][name=%s]",__func__,cfgfile,lineno,field,line);
			fclose(fp);
			return -1;
		}
		if(strlen(line)==0){
			think_error(0,"[%s]:name is empty![%s:%d][field=%s]",__func__,cfgfile,lineno,field);
			fclose(fp);
			return -1;
		}
		strcpy(name,line);
		/* value */
		bzero(value,sizeof(value));
		memset(line,' ',r+1);	/* r used here */
		think_strtrim(line);
		if(*line=='$'){
			if(think_chrstat(line,'$')!=1){
				think_error(0,"[%s]:too many '$' in value![%s:%d][field=%s][name=%s]",__func__,cfgfile,lineno,field,name);
				fclose(fp);
				return -1;
			}
			line[0]=' ';
			think_strtrim(line);
			if(strlen(line)>=THINK_CFG_VALUE_SIZE){
				think_error(0,"[%s]:value too long![%s:%d][field=%s][name=%s]",__func__,cfgfile,lineno,field,name);
				fclose(fp);
				return -1;
			}
			if(think_getcfg(cfg,field,line,'C',value,sizeof(value),0)<0){
				think_errorerror();
				fclose(fp);
				return -1;
			}
		}else{
			if(strlen(line)>=THINK_CFG_VALUE_SIZE){
				think_error(0,"[%s]:value too long![%s:%d][field=%s][name=%s]",__func__,cfgfile,lineno,field,name);
				fclose(fp);
				return -1;
			}
			strcpy(value,line);
		}
		/* putcfg */
		if(__think_putcfg(cfg,field,name,value,cfgfile,lineno)<0){
			think_errorerror();
			fclose(fp);
			return -1;
		}
		cfg->cfgcount++;
		includeflag=0;
	}
	fclose(fp);

	return 0;
}
int __think_putcfg(THINK_CFG *cfg,const char *field,const char *name,const char *value,const char *file,unsigned int lineno)
{
	THINK_CFG_CFGLIST *p,*first,*last,*cfglist;

	cfglist=cfg->cfglist;
	if((p=__think_findcfg(cfg,field,name))!=NULL){
		strcpy(p->value,value);
		return 0;
	}
	if((p=malloc(sizeof(THINK_CFG_CFGLIST)))==NULL){
		think_error(0,"[%s]:malloc error![%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
	memset(p,0x00,sizeof(THINK_CFG_CFGLIST));
	strcpy(p->field,field);
	strcpy(p->name,name);
	strcpy(p->value,value);
	strcpy(p->file,file);
	p->lineno=lineno;
	p->prior=NULL;
	p->next=NULL;
	if(cfglist==NULL){
		p->prior=p;
		p->next=p;
		cfg->cfglist=p;	/* cfg->cfglist */
	}else{
		first=cfglist;
		last=cfglist->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}

	return 0;
}
THINK_CFG_CFGLIST *__think_findcfg(THINK_CFG *cfg,const char *field,const char *name)
{
	THINK_CFG_CFGLIST *p;

	p=cfg->cfglist;
	if(p==NULL){
		think_error(0,"[%s]:cfglist is empty.",__func__);
		return NULL;
	}
	do{
		if(strcmp(p->field,field)==0 && strcmp(p->name,name)==0)
			return p;
		p=p->next;
	}while(p!=cfg->cfglist);

	think_error(0,"[%s]:config not exists.[field=%s][name=%s]",__func__,field,name);
	return NULL;
}
int think_getcfg(THINK_CFG *cfg,const char *field,const char *name,char type,void *value,unsigned int size,int flags)
{
	THINK_CFG_CFGLIST *p;
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if((p=__think_findcfg(cfg,field,name))==NULL){
		if(strlen(field)>0){
			/* continue to find global cfg */
			if((p=__think_findcfg(cfg,"",name))==NULL){
				if(flags & THINK_CFG_NOT_IGNORE){
					think_error(0,"[%s]:config not exists![%s]->[%s]",__func__,field,name);
					return -1;
				}
				return 0;	/* ignore */
			}
		}else{
			if(flags & THINK_CFG_NOT_IGNORE){
				think_error(0,"[%s]:config not exists![%s]->[%s]",__func__,field,name);
				return -1;
			}
			return 0;	/* ignore */
		}
	}
	if(strlen(p->value)==0 && (flags & THINK_CFG_NOT_NULL)){
		think_error(0,"[%s]:[%s]->[%s] is empty.[%s:%d]",__func__,p->field,p->name,p->file,p->lineno);
		return -1;
	}
	if(value==NULL)
		return 0;
	switch(type){
		case THINK_CFG_CHAR:	/* char */
			if(strlen(p->value)>size){
				think_error(0,"[%s]:value too long![size=%d][len=%d][%s]->[%s]:[%s][%s:%d]",__func__,size,strlen(p->value),p->field,p->name,p->value,p->file,p->lineno);
				return -1;
			}
			if(strlen(p->value)==size && !(flags & THINK_CFG_NOT_STRING)){
				think_error(0,"[%s]:value equal to size![size=%d][%s]->[%s]:[%s][%s:%d]",__func__,size,p->field,p->name,p->value,p->file,p->lineno);
				return -1;
			}
			strncpy(value,p->value,size);
			return strlen(p->value);
		case THINK_CFG_SHORT:	/* short */
			t_short=atol(p->value);
			if(t_short==0 && (flags & THINK_CFG_NOT_NULL)){
				think_error(0,"[%s]:[%s]->[%s] is 0.[%s:%d]",__func__,p->field,p->name,p->file,p->lineno);
				return -1;
			}
			memcpy(value,&t_short,2);
			break;
		case THINK_CFG_INT:	/* int */
			t_int=atol(p->value);
			if(t_int==0 && (flags & THINK_CFG_NOT_NULL)){
				think_error(0,"[%s]:[%s]->[%s] is 0.[%s:%d]",__func__,p->field,p->name,p->file,p->lineno);
				return -1;
			}
			memcpy(value,&t_int,4);
			break;
		case THINK_CFG_LONG:	/* long */
			t_long=atoll(p->value);
			if(t_long==0 && (flags & THINK_CFG_NOT_NULL)){
				think_error(0,"[%s]:[%s]->[%s] is 0.[%s:%d]",__func__,p->field,p->name,p->file,p->lineno);
				return -1;
			}
			memcpy(value,&t_long,8);
			break;
		case THINK_CFG_FLOAT:	/* float */
			t_float=atof(p->value);
			if(t_float==0.0 && (flags & THINK_CFG_NOT_NULL)){
				think_error(0,"[%s]:[%s]->[%s] is 0.[%s:%d]",__func__,p->field,p->name,p->file,p->lineno);
				return -1;
			}
			memcpy(value,&t_float,4);
			break;
		case THINK_CFG_DOUBLE:	/* double */
			t_double=atof(p->value);
			if(t_double==0.0 && (flags & THINK_CFG_NOT_NULL)){
				think_error(0,"[%s]:[%s]->[%s] is 0.[%s:%d]",__func__,p->field,p->name,p->file,p->lineno);
				return -1;
			}
			memcpy(value,&t_double,8);
			break;
		default:
			think_error(0,"[%s]:illegal type![%s]->[%s][type=%c]",__func__,field,name,type);
			return -1;
	}

	return 0;
}
int think_showcfg(THINK_CFG *cfg)
{
	THINK_CFG_CFGLIST *p;

	if(cfg->cfgcount==0){
		printf("cfglist is empty!\n");
		return 0;
	}
	p=cfg->cfglist;
	do{
		printf("[%s]:[%s=%s]\n",p->field,p->name,p->value);
		p=p->next;
	}while(p!=cfg->cfglist);

	return 0;
}
int think_freecfg(THINK_CFG *cfg)
{
	THINK_CFG_CFGLIST *p;

	while(cfg->cfglist){
		p=cfg->cfglist;
		if(p==p->next){
			free(p);
			break;
		}
		p->next->prior=p->prior;
		p->prior->next=p->next;
		cfg->cfglist=p->next;
		free(p);
	}
	free(cfg);

	return 0;
}
int think_getcfgs(THINK_CFG *cfg,THINK_CFGARRAY *cfgarray,unsigned int cfgcount)
{
	THINK_CFGARRAY *p;
	int i;
	THINK_CFG_CFGLIST *c;

	/* init */
	p=cfgarray;
	for(i=0;i<cfgcount;i++,p++){
		if(p->exists)
			*p->exists=0;
	}

	p=cfgarray;
	for(i=0;i<cfgcount;i++,p++){
		if((c=__think_findcfg(cfg,p->domain,p->name))==NULL){
			if(strlen(p->domain)>0){
				/* continue to find global cfg */
				if((c=__think_findcfg(cfg,"",p->name))==NULL){
					if(p->flags & THINK_CFG_NOT_IGNORE){
						think_error(0,"[%s]:config not exists![%s]->[%s]",__func__,p->domain,p->name);
						return -1;
					}
					continue;	/* ignore */
				}
			}else{
				if(p->flags & THINK_CFG_NOT_IGNORE){
					think_error(0,"[%s]:config not exists![%s]->[%s]",__func__,p->domain,p->name);
					return -1;
				}
				continue;	/* ignore */
			}
		}
		if(think_getcfg(cfg,p->domain,p->name,p->type,p->value,p->size,p->flags)<0){
			think_errorerror();
			return -1;
		}
		if(p->exists)
			*p->exists=1;
	}

	return 0;
}
