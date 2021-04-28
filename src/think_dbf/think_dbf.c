#include "think_dbf.h"

/* think_dbf_open */
THINK_DBF *think_dbf_open(const char *pathname,int flags,const char *conffile);

/* think_dbf_fetch */
int think_dbf_fetch(THINK_DBF *dbf);

/* think_dbf_getfield */
int think_dbf_getfield(THINK_DBF *dbf,unsigned int no,char type,void *value,unsigned int size,int flags);

/* think_dbf_getfieldbyname */
int think_dbf_getfieldbyname(THINK_DBF *dbf,const char *name,char type,void *value,unsigned int size,int flags);

/* think_dbf_new */
void think_dbf_new(THINK_DBF *dbf);

/* think_dbf_putfield */
int think_dbf_putfield(THINK_DBF *dbf,unsigned int no,char type,const void *value,unsigned int length,int flags);

/* think_dbf_putfieldbyname */
int think_dbf_putfieldbyname(THINK_DBF *dbf,const char *name,char type,const void *value,unsigned int length,int flags);

/* think_dbf_insert */
int think_dbf_insert(THINK_DBF *dbf);

/* think_dbf_seek */
int think_dbf_seek(THINK_DBF *dbf,int offset,int whence);

/* think_dbf_sync */
int think_dbf_sync(THINK_DBF *dbf);

/* think_dbf_eof */
int think_dbf_eof(THINK_DBF *dbf);

/* think_dbf_close */
int think_dbf_close(THINK_DBF *dbf);

int think_dbf_seek(THINK_DBF *dbf,int offset,int whence)
{
	int pos;

	switch(whence){
		case THINK_DBF_SEEK_SET:
			if(offset<0){
				offset=0;
			}else if(offset>dbf->record_count){
				offset=dbf->record_count;
			}
			break;
		case THINK_DBF_SEEK_CUR:
			if(dbf->cursor==0){
				if(offset<0){
					offset=0;
				}else if(offset>dbf->record_count){
					offset=dbf->record_count;
				}
			}else if(think_dbf_eof(dbf) || dbf->cursor==dbf->record_count){
				if(offset>=0){
					offset=dbf->record_count;
				}else{
					if(abs(offset)>dbf->record_count){
						offset=0;
					}else{
						offset=dbf->record_count-offset*(-1)+1;
					}
				}
			}else{
				if(offset>0){
					if(offset>dbf->record_count-dbf->cursor){
						offset=dbf->record_count;
					}else{
						offset=dbf->cursor+offset;
					}
				}else if(offset==0){
					offset=dbf->cursor;
				}else{
					if(offset<dbf->cursor*(-1)){
						offset=0;
					}else{
						offset=dbf->cursor+offset;
					}
				}
			}
			break;
		case THINK_DBF_SEEK_END:
			if(offset>=0){
				offset=dbf->record_count;
			}else{
				if(abs(offset)>dbf->record_count){
					offset=0;
				}else{
					offset=dbf->record_count-offset*(-1)+1;
				}
			}
			break;
		default:
			think_error(0,"[%s]:illegal whence value.[whence=%d]",__func__,whence);
			return -1;
	};
	pos=offset*dbf->record_length+dbf->head_length;
	if(seek(dbf->fd,pos,SEEK_SET)<0){
		think_error(0,"[%s]:seek error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
	dbf->cursor=offset;

	return 0;
}

int think_dbf_close(THINK_DBF *dbf)
{
	if(!(dbf->flags & THINK_DBF_RDONLY) && !(dbf->flags & THINK_DBF_SYNC))
		think_dbf_sync(dbf);
	close(dbf->fd);
	free(dbf->buffer);
	free(dbf->field_array);
	free(dbf);

	return 0;
}
THINK_DBF *think_dbf_open(const char *pathname,int flags,const char *conffile)
{
	THINK_DBF *dbf;
	THINK_CONF *conf;
	THINK_DBF_FIELD *p;
	int oflags,i,pos,h,len,dbf_create;
	char buffer[32];
	struct tm *t;
	time_t tt;


	/* dbf handle */
	if((dbf=malloc(sizeof(THINK_DBF)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}
	bzero(dbf,sizeof(THINK_DBF));

	/* summary info */
	dbf->record_count=0;
	dbf->head_length=32+1;	/* file head,field end */
	dbf->field_count=0;
	dbf->record_length=1;	/* delete flag */

	/* cursor set */
	dbf->cursor=0;

	/* creat & excl & trunc */
	dbf_create=0;	/* file create or not */
	if(flags & THINK_DBF_CREAT){
		if(access(pathname,F_OK)==0){
			if(flags & THINK_DBF_EXCL){
				think_error(0,"[%s]:file exists.",__func__);
				free(dbf);
				return NULL;
			}
		}else{
			dbf_create=1;	/* create */
		}
	}
	if(flags & THINK_DBF_TRUNC)
		dbf_create=1;	/* create */

	/* open flags */
	dbf->flags=flags;
	if(flags==0)
		dbf->flags=THINK_DBF_RDWR;	/* defalut flags */
	oflags=0;
	if(dbf->flags & THINK_DBF_CREAT)	/* creat */
		oflags|=O_CREAT;
	if(dbf->flags & THINK_DBF_TRUNC)	/* trunc */
		oflags|=O_TRUNC;
	oflags|=O_RDWR|_O_BINARY;
	if((dbf->fd=open(pathname,oflags,0666))<0){
		think_error(0,"[%s]:open error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(dbf);
		return NULL;
	}

	if(dbf_create){	/* create */
		/* load conf */
		if((conf=think_loadconf(conffile,'|'))==NULL){
			think_errorerror();
			close(dbf->fd);
			free(dbf);
			return NULL;
		}
		if(conf->hcount==0){
			think_error(0,"[%s]:no fields.",__func__);
			think_freeconf(conf);
			close(dbf->fd);
			free(dbf);
			return NULL;
		}


		/* summary info */
		tt=time(NULL);
		t=localtime(&tt);
		sprintf(dbf->last_modify_date,"%04d-%02d-%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
		for(h=0;h<conf->hcount;h++){
			dbf->field_count++;
			dbf->head_length+=32;
			if(think_getconf(conf,h,2,'I',&len,0,0)<0){
				think_errorerror();
				think_freeconf(conf);
				close(dbf->fd);
				free(dbf);
				return NULL;
			}
			dbf->record_length+=len;
		}

		/* init field array */
		if((dbf->field_array=malloc(sizeof(THINK_DBF_FIELD)*dbf->field_count))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			think_freeconf(conf);
			close(dbf->fd);
			free(dbf);
			return NULL;
		}
		bzero(dbf->field_array,sizeof(THINK_DBF_FIELD)*dbf->field_count);

		pos=1;	/* 0 used for delete flag */
		for(h=0,p=dbf->field_array;h<conf->hcount;h++,p++){
			if(think_getconf(conf,h,0,'C',p->name,sizeof(p->name),0)<0){
				think_errorerror();
				free(dbf->field_array);
				think_freeconf(conf);
				close(dbf->fd);
				free(dbf);
				return NULL;
			}
			if(think_getconf(conf,h,1,'C',&p->type,1,THINK_CONF_NOT_STRING)<0){
				think_errorerror();
				free(dbf->field_array);
				think_freeconf(conf);
				close(dbf->fd);
				free(dbf);
				return NULL;
			}
			p->offset=pos;
			if(think_getconf(conf,h,2,'I',&p->length,0,0)<0){
				think_errorerror();
				free(dbf->field_array);
				think_freeconf(conf);
				close(dbf->fd);
				free(dbf);
				return NULL;
			}
			pos+=p->length;
		}

		/* freeconf */
		think_freeconf(conf);

		/* sync */
		if(dbf->flags & THINK_DBF_SYNC){
			if(think_dbf_sync(dbf)<0){
				think_errorerror();
				free(dbf->field_array);
				close(dbf->fd);
				free(dbf);
				return NULL;
			}
		}
	}else{	/* open */
		/* summary */
		if(seek(dbf->fd,0,SEEK_SET)<0){
			think_error(0,"[%s]:seek error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(dbf->fd);
			free(dbf);
			return NULL;
		}
		/* read */
		if(read(dbf->fd,buffer,32)!=32){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(dbf->fd);
			free(dbf);
			return NULL;
		}

		/* file type */
		if(buffer[0]!=0x03){
			think_error(0,"[%s]:file type not supported![filetype=0x%0X]",__func__,buffer[0]);
			close(dbf->fd);
			free(dbf);
			return NULL;
		}
		/*last modify date*/
		strcpy(dbf->last_modify_date,"2000-00-00");
		think_bintohex(dbf->last_modify_date+2,*(buffer+1));
		think_bintohex(dbf->last_modify_date+5,*(buffer+2));
		think_bintohex(dbf->last_modify_date+8,*(buffer+3));

		/*record count*/
		dbf->record_count=think_ltoh4(*((THINK_UINT *)(buffer+4)));

		/*head length*/
		dbf->head_length=think_ltoh2(*((THINK_USHORT *)(buffer+8)));

		/*record lenght*/
		dbf->record_length=think_ltoh2(*((THINK_USHORT *)(buffer+10)));

		/*field count*/
		dbf->field_count=(dbf->head_length-1-32)/32;

		/* fields */
		if((dbf->field_array=malloc(sizeof(THINK_DBF_FIELD)*dbf->field_count))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(dbf->fd);
			free(dbf);
			return NULL;
		}
		bzero(dbf->field_array,sizeof(THINK_DBF_FIELD)*dbf->field_count);
		if(seek(dbf->fd,32,SEEK_SET)<0){
			think_error(0,"[%s]:seek error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			free(dbf->field_array);
			close(dbf->fd);
			free(dbf);
			return NULL;
		}
		pos=1;	/* 0 used for delete flag */
		for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
			if(read(dbf->fd,buffer,32)!=32){
				think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				free(dbf->field_array);
				close(dbf->fd);
				free(dbf);
				return NULL;
			}

			/* field name */
			snprintf(p->name,sizeof(p->name),"%s",buffer);
			think_strtrim(p->name);

			/* field type */
			p->type=buffer[11];

			/*field offset*/
			p->offset=pos;

			/*field length*/
			p->length=(unsigned char)buffer[16];

			pos+=p->length;
		}

		/* dbf head end flag */
		if(read(dbf->fd,buffer,1)!=1){
			think_error(0,"[%s]:seek error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			free(dbf->field_array);
			close(dbf->fd);
			free(dbf);
			return NULL;
		}
		if(buffer[0]!=0x0D){
			think_error(0,"[%s]:dbf head end flag not match.",__func__);
			free(dbf->field_array);
			close(dbf->fd);
			free(dbf);
			return NULL;
		}
	}
	if((dbf->buffer=malloc(dbf->record_length))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(dbf->field_array);
		close(dbf->fd);
		free(dbf);
		return NULL;
	}

	return dbf;
}
int think_dbf_sync(THINK_DBF *dbf)
{
	THINK_DBF_FIELD *p;
	char buffer[32];
	int i,offset,whence;
	struct tm *t;
	time_t tt;

	/* seek */
	if(seek(dbf->fd,0,SEEK_SET)<0){
		think_error(0,"[%s]:seek error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
	/* write dbf summary */
	bzero(buffer,sizeof(buffer));

	/* file type: 0 */
	buffer[0]=0x03;

	/* last modify date: 1 */
	tt=time(NULL);
	t=localtime(&tt);
	sprintf(dbf->last_modify_date,"%04d-%02d-%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
	think_hextobin(buffer+1,dbf->last_modify_date+2);	/* yy */
	think_hextobin(buffer+2,dbf->last_modify_date+5);	/* mm */
	think_hextobin(buffer+3,dbf->last_modify_date+8);	/* dd */

	/* record count: 4 */
	*((THINK_UINT *)(buffer+4))=think_htol4(dbf->record_count);

	/* head length: 8 */
	*((THINK_USHORT *)(buffer+8))=think_htol2(dbf->head_length);

	/* record lenght: 10 */
	*((THINK_USHORT *)(buffer+10))=think_htol2(dbf->record_length);

	/* write */
	if(write(dbf->fd,buffer,32)!=32){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* write dbf fields */
	for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
		bzero(buffer,sizeof(buffer));

		/*field name*/
		snprintf(buffer,11,"%s",p->name);

		/*field type*/
		buffer[11]=p->type;

		/*field offset*/
		*((THINK_UINT *)(buffer+12))=think_htol4(p->offset);

		/*field length*/
		*((unsigned char *)buffer+16)=p->length;

		/* write */
		if(write(dbf->fd,buffer,32)!=32){
			think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
	}

	/* write dbf head end flag */
	buffer[0]=0x0D;
	if(write(dbf->fd,buffer,1)!=1){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* cursor reset */
	if(think_dbf_eof(dbf)){
		offset=0;
		whence=THINK_DBF_SEEK_END;
	}else{
		offset=dbf->cursor;
		whence=THINK_DBF_SEEK_SET;
	}
	if(think_dbf_seek(dbf,offset,whence)<0){
		think_errorerror();
		return -1;
	}

	return 0;
}
int think_dbf_fetch(THINK_DBF *dbf)
{
	if(dbf->flags & THINK_DBF_WRONLY){
		think_error(0,"[%s]:write only.",__func__);
		return -1;
	}
	if(think_dbf_eof(dbf)){
		think_error(0,"[%s]:record end.",__func__);
		return -1;
	}
	if(dbf->record_count==0 || dbf->cursor==dbf->record_count){
		dbf->cursor=THINK_DBF_EOF;
		return 0;
	}
	if(read(dbf->fd,dbf->buffer,dbf->record_length)!=dbf->record_length){
		think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
	dbf->cursor++;

	return 1;
}
void think_dbf_new(THINK_DBF *dbf)
{
	dbf->buffer[0]=0x20;
	bzero(dbf->buffer+1,dbf->record_length-1);
}
int think_dbf_insert(THINK_DBF *dbf)
{
	if(dbf->flags & THINK_DBF_RDONLY){
		think_error(0,"[%s]:read only.",__func__);
		return -1;
	}

	/* append */
	if(dbf->flags & THINK_DBF_APPEND){
		if(think_dbf_seek(dbf,0,THINK_DBF_SEEK_END)<0){
			think_errorerror();
			return -1;
		}
	}
	if(write(dbf->fd,dbf->buffer,dbf->record_length)!=dbf->record_length){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* record count */
	if(think_dbf_eof(dbf) || dbf->cursor==dbf->record_count)	/* else is update */
		dbf->record_count++;

	/* cursor */
	if(think_dbf_eof(dbf))
		dbf->cursor=dbf->record_count;
	else
		dbf->cursor++;

	if(dbf->flags & THINK_DBF_SYNC){
		if(think_dbf_sync(dbf)<0)
			return -1;
	}

	return 0;
}
int think_dbf_getfield(THINK_DBF *dbf,unsigned int no,char type,void *value,unsigned int size,int flags)
{
	THINK_DBF_FIELD *p;
	char fieldvalue[256];
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if(no>=dbf->field_count){
		think_error(0,"[%s]:field no out of range.[no=%d]",__func__);
		return -1;
	}
	p=dbf->field_array+no;
	bzero(fieldvalue,sizeof(fieldvalue));
	memcpy(fieldvalue,dbf->buffer+p->offset,p->length);
	if(strlen(fieldvalue)==0 && (flags & THINK_DBF_NOT_NULL)){
		think_error(0,"[%s]:field value is empty.[%d]->[%d:%s]",__func__,dbf->cursor,no,p->name);
		return -1;
	}
	switch(type){
		case THINK_DBF_CHAR:		/* char */
			if(strlen(fieldvalue)>size){
				think_error(0,"[%s]:value too long![size=%d][len=%d][%d]->[%d:%s]:[%s]",__func__,size,strlen(fieldvalue),dbf->cursor,no,p->name,fieldvalue);
				return -1;
			}
			if(strlen(fieldvalue)==size && (!flags & THINK_DBF_NOT_STRING)){
				think_error(0,"[%s]:value len equal to size![size=%d][%d]->[%d:%s]:[%s]",__func__,size,dbf->cursor,no,p->name,fieldvalue);
				return -1;
			}
			strncpy(value,fieldvalue,size);
			if(flags & THINK_DBF_NOT_STRING)
				return strlen(fieldvalue);
			if(!(flags & THINK_DBF_NOT_TRIM))
				think_strtrim(value);
			if(strlen(value)==0 && (flags & THINK_DBF_NOT_NULL)){
				think_error(0,"[%s]:field value is empty.[%d]->[%d:%s]",__func__,dbf->cursor,no,p->name);
				return -1;
			}
			return strlen(value);
		case THINK_DB_SHORT:		/* short */
			t_short=atol(fieldvalue);
			if(t_short==0 && (flags & THINK_DBF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,dbf->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_short,2);
			break;
		case THINK_DB_INT:		/* int */
			t_int=atol(fieldvalue);
			if(t_int==0 && (flags & THINK_DBF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,dbf->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_int,4);
			break;
		case THINK_DB_LONG:		/* long */
			t_long=atoll(fieldvalue);
			if(t_long==0 && (flags & THINK_DBF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,dbf->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_long,8);
			break;
		case THINK_DB_FLOAT:		/* float */
			t_float=atof(fieldvalue);
			if(t_float==0.0 && (flags & THINK_DBF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,dbf->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_float,4);
			break;
		case THINK_DB_DOUBLE:		/* double */
			t_double=atof(fieldvalue);
			if(t_double==0.0 && (flags & THINK_DBF_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,dbf->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_double,8);
			break;
		default:
			think_error(0,"[%s]:illegal field type.[fieldno=%d][type='%c']",__func__,no,type);
			return -1;
	}

	return 0;
}
int think_dbf_getfieldbyname(THINK_DBF *dbf,const char *name,char type,void *value,unsigned int size,int flags)
{
	THINK_DBF_FIELD *p;
	int i,r;

	for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
		if(strcmp(name,p->name)==0)
			break;
	}
	if(i==dbf->field_count){
		think_error(0,"[%s]:field not exists.[name=%s]",__func__,name);
		return -1;
	}
	if((r=think_dbf_getfield(dbf,i,type,value,size,flags))<0){
		think_errorerror();
		return -1;
	}

	return r;
}
int think_dbf_putfield(THINK_DBF *dbf,unsigned int no,char type,const void *value,unsigned int length,int flags)
{
	THINK_DBF_FIELD *p;
	char fieldvalue[256];
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if(no>=dbf->field_count){
		think_error(0,"[%s]:field no out of range.[no=%d]",__func__);
		return -1;
	}
	p=dbf->field_array+no;
	bzero(fieldvalue,sizeof(fieldvalue));
	switch(type){
		case THINK_DBF_CHAR:
			if(length>p->length){
				think_error(0,"[%s]:value too long![%d:%s],[type=char],[length=%d],[len=%d]",__func__,no,p->name,p->length,length);
				return -1;
			}
			memcpy(fieldvalue,value,p->length);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case THINK_DB_SHORT:
			memcpy(&t_short,value,2);
			sprintf(fieldvalue,"%d",t_short);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case THINK_DB_INT:
			memcpy(&t_int,value,4);
			sprintf(fieldvalue,"%d",t_int);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case THINK_DB_LONG:
			memcpy(&t_long,value,8);
			sprintf(fieldvalue,"%lld",t_long);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case THINK_DB_FLOAT:
			memcpy(&t_float,value,4);
			sprintf(fieldvalue,"%f",t_float);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case THINK_DB_DOUBLE:
			memcpy(&t_double,value,8);
			sprintf(fieldvalue,"%f",t_double);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		default:
			think_error(0,"[%s]:illegal field type.[fieldno=%d][type='%c']",__func__,no,type);
			return -1;
	};

	return 0;
}
int think_dbf_putfieldbyname(THINK_DBF *dbf,const char *name,char type,const void *value,unsigned int length,int flags)
{
	THINK_DBF_FIELD *p;
	int i;

	for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
		if(strcmp(name,p->name)==0)
			break;
	}
	if(i==dbf->field_count){
		think_error(0,"[%s]:field not exists.[name=%s]",__func__,name);
		return -1;
	}
	if(think_dbf_putfield(dbf,i,type,value,length,flags)<0){
		think_errorerror();
		return -1;
	}

	return 0;
}

int think_dbf_eof(THINK_DBF *dbf)
{
	if(dbf->cursor==THINK_DBF_EOF)
		return 1;

	return 0;
}
