#include "think_os.h"
#include "think_error.h"
#include "think_utility.h"
#include "think_conf.h"
#include "think_db.h"

int think_db_seek(THINK_DB *db,int offset,int whence)
{
	int pos;

	switch(whence){
		case THINK_DB_SEEK_SET:
			if(offset<0){
				offset=0;
			}else if(offset>db->record_count){
				offset=db->record_count;
			}
			break;
		case THINK_DB_SEEK_CUR:
			if(db->cursor==0){
				if(offset<0){
					offset=0;
				}else if(offset>db->record_count){
					offset=db->record_count;
				}
			}else if(think_db_eof(db) || db->cursor==db->record_count){
				if(offset>=0){
					offset=db->record_count;
				}else{
					if(abs(offset)>db->record_count){
						offset=0;
					}else{
						offset=db->record_count-offset*(-1)+1;
					}
				}
			}else{
				if(offset>0){
					if(offset>db->record_count-db->cursor){
						offset=db->record_count;
					}else{
						offset=db->cursor+offset;
					}
				}else if(offset==0){
					offset=db->cursor;
				}else{
					if(offset<db->cursor*(-1)){
						offset=0;
					}else{
						offset=db->cursor+offset;
					}
				}
			}
			break;
		case THINK_DB_SEEK_END:
			if(offset>=0){
				offset=db->record_count;
			}else{
				if(abs(offset)>db->record_count){
					offset=0;
				}else{
					offset=db->record_count-offset*(-1)+1;
				}
			}
			break;
		default:
			think_error(0,"[%s]:illegal whence value.[whence=%d]",__func__,whence);
			return -1;
	};
	pos=offset*db->record_length+db->head_length;
	if(seek(db->fd,pos,SEEK_SET)<0){
		think_error(0,"[%s]:seek error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}
	db->cursor=offset;

	return 0;
}

int think_db_close(THINK_DB *db)
{
	if(!(db->flags & THINK_DB_RDONLY) && !(db->flags & THINK_DB_SYNC))
		think_db_sync(db);
	close(db->fd);
	free(db->record_buffer);
	free(db->option_buffer);
	free(db->field_array);
	free(db->offset_array);
	free(db);

	return 0;
}
THINK_DB *think_db_open(const char *pathname,int flags,THINK_DB_FIELD *fieldarray,unsigned int fieldcount)
{
	THINK_DB *db;
	THINK_DB_FIELD *p;
	int oflags,i,db_create;
	struct tm *t;
	time_t tt;
	unsigned char nlen;
	char tmpstr[128];

	/* db handle */
	if((db=malloc(sizeof(THINK_DB)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}
	bzero(db,sizeof(THINK_DB));

	/* init */

	/* creat & excl & trunc */
	db_create=0;	/* file create or not */
	if(flags & THINK_DB_CREAT){
		if(access(pathname,F_OK)==0){
			if(flags & THINK_DB_EXCL){
				think_error(0,"[%s]:file exists.",__func__);
				free(db);
				return NULL;
			}
		}else{
			db_create=1;	/* create */
		}
	}
	if(flags & THINK_DB_TRUNC)
		db_create=1;	/* create */

	/* open flags */
	db->flags=flags;
	if(flags==0)
		db->flags=THINK_DB_RDWR;	/* defalut flags */
	oflags=0;
	if(db->flags & THINK_DB_CREAT)	/* creat */
		oflags|=O_CREAT;
	if(db->flags & THINK_DB_TRUNC)	/* trunc */
		oflags|=O_TRUNC;
	oflags|=O_RDWR|_O_BINARY;
	if((db->fd=open(pathname,oflags,0666))<0){
		think_error(0,"[%s]:open error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		free(db);
		return NULL;
	}

	if(db_create){	/* create */
		if(fieldcount==0){
			think_error(0,"[%s]:fieldcount is 0.",__func__);
			close(db->fd);
			free(db);
			return NULL;
		}

		/* type */
		db->type=0;
		db->head_length++;

		/* version */
		db->version=0;
		db->head_length++;

		/* field count */
		db->field_count=fieldcount;
		db->head_length++;

		/* field array */
		if((db->field_array=malloc(sizeof(THINK_DB_FIELD)*fieldcount))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db);
			return NULL;
		}
		memcpy(db->field_array,fieldarray,sizeof(THINK_DB_FIELD)*fieldcount);

		/* offset array */
		if((db->offset_array=malloc(sizeof(int)*db->field_count))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db);
			return NULL;
		}
		db->record_length=1;	/* record flag */
		for(i=0,p=db->field_array;i<db->field_count;i++,p++){
			db->offset_array[i]=db->record_length;
			db->head_length++;	/* length of name */
			db->head_length+=strlen(p->name);	/* field name */
			switch(p->type){
				case 'C':
					db->record_length+=p->length;
					break;
				case 'S':
					db->record_length+=2;
					break;
				case 'I':
					db->record_length+=4;
					break;
				case 'L':
					db->record_length+=8;
					break;
				case 'F':
					db->record_length+=4;
					break;
				case 'D':
					db->record_length+=8;
					break;
				default:
					think_error(0,"[%s]:illegal field type.[type='%c']",__func__,p->type);
					close(db->fd);
					free(db->field_array);
					free(db->offset_array);
					free(db);
					return NULL;
			}
			db->head_length++;	/* field type */
			db->head_length+=4;	/* field length */
			db->head_length+=4;	/* field flags */
		}
		if((db->record_buffer=malloc(db->record_length))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db->offset_array);
			free(db);
			return NULL;
		}

		/* created time */
		tt=time(NULL);
		t=localtime(&tt);
		sprintf(db->created_time,"%04d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		db->head_length+=14;

		/* last modified time */
		strcpy(db->last_modified_time,db->created_time);
		db->head_length+=14;

		/* record count */
		db->record_count=0;
		db->head_length+=4;

		/* transaction flag */
		db->transaction_flag=0;
		db->head_length++;

		/* option length */
		db->option_length=0;
		db->head_length+=4;

		/* option buffer */
		db->option_buffer=NULL;

		/* sync */
		if(think_db_sync(db)<0){
			think_errorerror();
			close(db->fd);
			free(db->field_array);
			free(db->offset_array);
			if(db->option_length)
				free(db->option_buffer);
			free(db);
			return NULL;
		}
	}else{	/* open */
		/* head length */
		db->head_length=0;

		if(seek(db->fd,0,SEEK_SET)<0){
			think_error(0,"[%s]:seek error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db);
			return NULL;
		}

		/* type */
		if(read(db->fd,&db->type,1)!=1){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db);
			return NULL;
		}
		if(db->type!=0){
			think_error(0,"[%s]:db type not supported.[type=%d]",__func__,db->type);
			close(db->fd);
			free(db);
			return NULL;
		}
		db->head_length++;

		/* version */
		if(read(db->fd,&db->version,1)!=1){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db);
			return NULL;
		}
		db->head_length++;

		/* field count */
		if(read(db->fd,&db->field_count,1)!=1){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db);
			return NULL;
		}
		db->head_length++;

		/* field array */
		if((db->field_array=malloc(sizeof(THINK_DB_FIELD)*db->field_count))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db);
			return NULL;
		}
		bzero(db->field_array,sizeof(THINK_DB_FIELD)*db->field_count);

		for(i=0,p=db->field_array;i<db->field_count;i++,p++){
			/* length of field name */
			if(read(db->fd,&nlen,1)!=1){
				think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				close(db->fd);
				free(db->field_array);
				free(db);
				return NULL;
			}
			db->head_length++;

			/* field name */
			if(read(db->fd,p->name,nlen)!=nlen){
				think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				close(db->fd);
				free(db->field_array);
				free(db);
				return NULL;
			}
			db->head_length+=nlen;

			/* field type */
			if(read(db->fd,&p->type,1)!=1){
				think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				close(db->fd);
				free(db->field_array);
				free(db);
				return NULL;
			}
			db->head_length++;

			/* field length */
			if(read(db->fd,&p->length,4)!=4){
				think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				close(db->fd);
				free(db->field_array);
				free(db);
				return NULL;
			}
			p->length=think_btoh4(p->length);
			db->head_length+=4;

			/* field flags */
			if(read(db->fd,&p->flags,4)!=4){
				think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				close(db->fd);
				free(db->field_array);
				free(db);
				return NULL;
			}
			p->flags=think_btoh4(p->flags);
			db->head_length+=4;
		}

		/* offset array */
		if((db->offset_array=malloc(sizeof(int)*db->field_count))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db);
			return NULL;
		}
		db->record_length=1;	/* record flag */
		for(i=0,p=db->field_array;i<db->field_count;i++,p++){
			db->offset_array[i]=db->record_length;
			switch(p->type){
				case 'C':
					db->record_length+=p->length;
					break;
				case 'S':
					db->record_length+=2;
					break;
				case 'I':
					db->record_length+=4;
					break;
				case 'L':
					db->record_length+=8;
					break;
				case 'F':
					db->record_length+=4;
					break;
				case 'D':
					db->record_length+=8;
					break;
				default:
					think_error(0,"[%s]:illegal field type.[type='%c']",__func__,p->type);
					close(db->fd);
					free(db->field_array);
					free(db->offset_array);
					free(db);
					return NULL;
			}
		}
		if((db->record_buffer=malloc(db->record_length))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db->offset_array);
			free(db);
			return NULL;
		}

		/* created time */
		strcpy(db->created_time,"1970-01-01 00:00:00");
		if(read(db->fd,tmpstr,14)!=14){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db->offset_array);
			free(db->record_buffer);
			free(db);
			return NULL;
		}
		memcpy(db->created_time,tmpstr,4);	/* year */
		memcpy(db->created_time+5,tmpstr+4,2);	/* month */
		memcpy(db->created_time+8,tmpstr+6,2);	/* day */
		memcpy(db->created_time+11,tmpstr+8,2);	/* hour */
		memcpy(db->created_time+14,tmpstr+10,2);	/* minute */
		memcpy(db->created_time+17,tmpstr+12,2);	/* second */
		db->head_length+=14;

		/* last modified time */
		strcpy(db->last_modified_time,"1970-01-01 00:00:00");
		if(read(db->fd,tmpstr,14)!=14){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db->offset_array);
			free(db->record_buffer);
			free(db);
			return NULL;
		}
		memcpy(db->last_modified_time,tmpstr,4);	/* year */
		memcpy(db->last_modified_time+5,tmpstr+4,2);	/* month */
		memcpy(db->last_modified_time+8,tmpstr+6,2);	/* day */
		memcpy(db->last_modified_time+11,tmpstr+8,2);	/* hour */
		memcpy(db->last_modified_time+14,tmpstr+10,2);	/* minute */
		memcpy(db->last_modified_time+17,tmpstr+12,2);	/* second */
		db->head_length+=14;

		/* record count */
		if(read(db->fd,&db->record_count,4)!=4){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db->offset_array);
			free(db->record_buffer);
			free(db);
			return NULL;
		}
		db->record_count=think_btoh4(db->record_count);
		db->head_length+=4;

		/* transaction flag */
		if(read(db->fd,&db->transaction_flag,1)!=1){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db->offset_array);
			free(db->record_buffer);
			free(db);
			return NULL;
		}
		db->head_length++;

		/* option length */
		if(read(db->fd,&db->option_length,4)!=4){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			close(db->fd);
			free(db->field_array);
			free(db->offset_array);
			free(db->record_buffer);
			free(db);
			return NULL;
		}
		db->head_length+=4;

		/* option buffer */
		if(db->option_length){
			if((db->option_buffer=malloc(db->option_length))==NULL){
				think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				close(db->fd);
				free(db->field_array);
				free(db->offset_array);
				free(db->record_buffer);
				free(db);
				return NULL;
			}
			if(read(db->fd,db->option_buffer,db->option_length)!=db->option_length){
				think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
				close(db->fd);
				free(db->field_array);
				free(db->offset_array);
				free(db->record_buffer);
				free(db->option_buffer);
				free(db);
				return NULL;
			}
			db->head_length+=db->option_length;
		}
	}

	return db;
}

THINK_DB *think_db_openx(const char *pathname,int flags,const char *conffile)
{
	THINK_DB_FIELD *p,*fieldarray=NULL;
	int h,db_create,fieldcount=0;
	THINK_CONF *conf;
	char tmpstr[128];
	
	/* creat & excl & trunc */
	db_create=0;	/* file create or not */
	if(flags & THINK_DB_CREAT){
		if(access(pathname,F_OK)==0){
			if(flags & THINK_DB_EXCL){
				think_error(0,"[%s]:file exists.",__func__);
				return NULL;
			}
		}else{
			db_create=1;	/* create */
		}
	}
	if(flags & THINK_DB_TRUNC)
		db_create=1;	/* create */
	
	if(db_create){	/* create */
		/* load conf */
		if((conf=think_loadconf(conffile,'|'))==NULL){
			think_errorerror();
			return NULL;
		}
		if(conf->hcount==0){
			think_error(0,"[%s]:no fields.",__func__);
			think_freeconf(conf);
			return NULL;
		}
		fieldcount=conf->hcount;
		
		/* init field array */
		if((fieldarray=malloc(sizeof(THINK_DB_FIELD)*fieldcount))==NULL){
			think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			think_freeconf(conf);
			return NULL;
		}
		bzero(fieldarray,sizeof(THINK_DB_FIELD)*fieldcount);
		
		for(h=0,p=fieldarray;h<fieldcount;h++,p++){
			if(think_getconf(conf,h,0,'C',p->name,sizeof(p->name),0)<0){
				think_errorerror();
				free(fieldarray);
				think_freeconf(conf);
				return NULL;
			}
			if(think_getconf(conf,h,1,'C',&p->type,1,THINK_CONF_NOT_STRING)<0){
				think_errorerror();
				free(fieldarray);
				think_freeconf(conf);
				return NULL;
			}
			if(think_getconf(conf,h,2,'I',&p->length,sizeof(p->length),0)<0){
				think_errorerror();
				free(fieldarray);
				think_freeconf(conf);
				return NULL;
			}
			if(think_getconf(conf,h,3,'C',tmpstr,sizeof(tmpstr),0)<0){
				think_errorerror();
				free(fieldarray);
				think_freeconf(conf);
				return NULL;
			}
			p->flags=think_db_getfieldflags(tmpstr);
		}
		
		/* freeconf */
		think_freeconf(conf);
	}
	return think_db_open(pathname,flags,fieldarray,fieldcount);
}

int think_db_sync(THINK_DB *db)
{
	THINK_DB_FIELD *p;
	unsigned char nlen;
	int i,offset,whence,ti;
	struct tm *t;
	time_t tt;
	char tmpstr[128];

	if(seek(db->fd,0,SEEK_SET)<0){
		think_error(0,"[%s]:seek error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* type */
	if(write(db->fd,&db->type,1)!=1){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* version */
	if(write(db->fd,&db->version,1)!=1){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* field count */
	if(write(db->fd,&db->field_count,1)!=1){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* field array */
	for(i=0,p=db->field_array;i<db->field_count;i++,p++){
		/* length of field name */
		nlen=strlen(p->name);
		if(write(db->fd,&nlen,1)!=1){
			think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}

		/* field name */
		if(write(db->fd,p->name,nlen)!=nlen){
			think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}

		/* field type */
		if(write(db->fd,&p->type,1)!=1){
			think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}

		/* field length */
		ti=p->length;
		ti=think_htob4(ti);
		if(write(db->fd,&ti,4)!=4){
			think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}

		/* field flags */
		ti=p->flags;
		ti=think_htob4(ti);
		if(write(db->fd,&ti,4)!=4){
			think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
	}

	/* created time */
	memcpy(tmpstr,db->created_time,4);	/* year */
	memcpy(tmpstr+4,db->created_time+5,2);	/* month */
	memcpy(tmpstr+6,db->created_time+8,2);	/* day */
	memcpy(tmpstr+8,db->created_time+11,2);	/* hour */
	memcpy(tmpstr+10,db->created_time+14,2);	/* minute */
	memcpy(tmpstr+12,db->created_time+17,2);	/* second */
	if(write(db->fd,tmpstr,14)!=14){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* last modified time */
	tt=time(NULL);
	t=localtime(&tt);
	sprintf(db->last_modified_time,"%04d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
	sprintf(tmpstr,"%04d%02d%02d%02d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
	if(write(db->fd,tmpstr,14)!=14){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* record count */
	i=db->record_count;
	i=think_htob4(i);
	if(write(db->fd,&i,4)!=4){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* transaction flag */
	db->transaction_flag=0;
	if(write(db->fd,&db->transaction_flag,1)!=1){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* option length */
	i=db->option_length;
	i=think_htob4(i);
	if(write(db->fd,&i,4)!=4){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* option buffer */
	if(db->option_length){
		if(write(db->fd,db->option_buffer,db->option_length)!=db->option_length){
			think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
	}
	/* cursor reset */
	if(think_db_eof(db)){
		offset=0;
		whence=THINK_DB_SEEK_END;
	}else{
		offset=db->cursor;
		whence=THINK_DB_SEEK_SET;
	}
	if(think_db_seek(db,offset,whence)<0){
		think_errorerror();
		return -1;
	}

	return 0;
}
int think_db_fetch(THINK_DB *db)
{
	if(db->flags & THINK_DB_WRONLY){
		think_error(0,"[%s]:write only.",__func__);
		return -1;
	}
	while(1){
		if(think_db_eof(db)){
			think_error(0,"[%s]:record end.",__func__);
			return -1;
		}
		if(db->record_count==0 || db->cursor==db->record_count){
			db->cursor=THINK_DB_EOF;
			return 0;
		}
		if(read(db->fd,db->record_buffer,db->record_length)!=db->record_length){
			think_error(0,"[%s]:read error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
			return -1;
		}
		db->cursor++;
		if(db->record_buffer[0]==0)
			break;
	}

	return 1;
}
void think_db_new(THINK_DB *db)
{
	db->record_buffer[0]=0;
	bzero(db->record_buffer+1,db->record_length-1);
}
int think_db_insert(THINK_DB *db)
{
	if(db->flags & THINK_DB_RDONLY){
		think_error(0,"[%s]:read only.",__func__);
		return -1;
	}

	/* append */
	if(db->flags & THINK_DB_APPEND){
		if(think_db_seek(db,0,THINK_DB_SEEK_END)<0){
			think_errorerror();
			return -1;
		}
	}
	if(write(db->fd,db->record_buffer,db->record_length)!=db->record_length){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* record count */
	if(think_db_eof(db) || db->cursor==db->record_count)	/* else is update */
		db->record_count++;

	/* cursor */
	if(think_db_eof(db))
		db->cursor=db->record_count;
	else
		db->cursor++;

	if(db->flags & THINK_DB_SYNC){
		if(think_db_sync(db)<0)
			return -1;
	}

	return 0;
}
int think_db_delete(THINK_DB *db)
{
	if(db->flags & THINK_DB_RDONLY){
		think_error(0,"[%s]:read only.",__func__);
		return -1;
	}

	/* record count */
	if(think_db_eof(db) || db->cursor==db->record_count)
		return -1;

	/* delete */
	db->record_buffer[0]=1;
	if(write(db->fd,db->record_buffer,1)!=1){
		think_error(0,"[%s]:write error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return -1;
	}

	/* reset cursor */
	think_db_seek(db,db->cursor,THINK_DB_SEEK_SET);

	if(db->flags & THINK_DB_SYNC){
		if(think_db_sync(db)<0)
			return -1;
	}

	return 0;
}
int think_db_getfield(THINK_DB *db,unsigned int no,void *value,unsigned int size,int flags)
{
	THINK_DB_FIELD *p;
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if(no>=db->field_count){
		think_error(0,"[%s]:field no out of range.[no=%d]",__func__);
		return -1;
	}
	p=db->field_array+no;
	switch(p->type){
		case THINK_DB_CHAR:		/* char */
			if(p->length>size){
				think_error(0,"[%s]:value too long![size=%d][len=%d][%d]->[%d:%s]",__func__,size,p->length,db->cursor,no,p->name);
				return -1;
			}
			memcpy(value,db->record_buffer+db->offset_array[no],p->length);
			if(flags & THINK_DB_NOT_STRING)
				return p->length;
			if(p->length==size){
				think_error(0,"[%s]:value size equal to len![size=%d][%d]->[%d:%s]",__func__,size,db->cursor,no,p->name);
				return -1;
			}
			*((char *)value+p->length)=0;
			if(!(flags & THINK_DB_NOT_TRIM))
				think_strtrim(value);
			if(strlen(value)==0 && (flags & THINK_DB_NOT_NULL)){
				think_error(0,"[%s]:field value is empty.[%d]->[%d:%s]",__func__,db->cursor,no,p->name);
				return -1;
			}
			return strlen(value);
		case THINK_DB_SHORT:		/* short */
			memcpy(&t_short,db->record_buffer+db->offset_array[no],2);
			t_short=think_btoh2(t_short);
			if(t_short==0 && (flags & THINK_DB_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,db->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_short,2);
			break;
		case THINK_DB_INT:		/* int */
			memcpy(&t_int,db->record_buffer+db->offset_array[no],4);
			t_int=think_btoh4(t_int);
			if(t_int==0 && (flags & THINK_DB_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,db->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_int,4);
			break;
		case THINK_DB_LONG:		/* long */
			memcpy(&t_long,db->record_buffer+db->offset_array[no],8);
			t_long=think_btoh8(t_long);
			if(t_long==0 && (flags & THINK_DB_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,db->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_long,8);
			break;
		case THINK_DB_FLOAT:		/* float */
			memcpy(&t_int,db->record_buffer+db->offset_array[no],4);
			t_int=think_btoh4(t_int);
			memcpy(&t_float,&t_int,4);
			if(t_float==0.0 && (flags & THINK_DB_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,db->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_float,4);
			break;
		case THINK_DB_DOUBLE:		/* double */
			memcpy(&t_long,db->record_buffer+db->offset_array[no],8);
			t_long=think_btoh8(t_long);
			memcpy(&t_double,&t_long,8);
			if(t_double==0.0 && (flags & THINK_DB_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[%d]->[%d:%s]",__func__,db->cursor,no,p->name);
				return -1;
			}
			memcpy(value,&t_double,8);
			break;
		default:
			break;
	}

	return 0;
}
int think_db_getfieldbyname(THINK_DB *db,const char *name,void *value,unsigned int size,int flags)
{
	THINK_DB_FIELD *p;
	int i,r;

	for(i=0,p=db->field_array;i<db->field_count;i++,p++){
		if(strcmp(name,p->name)==0)
			break;
	}
	if(i==db->field_count){
		think_error(0,"[%s]:field not exists.[name=%s]",__func__,name);
		return -1;
	}
	if((r=think_db_getfield(db,i,value,size,flags))<0){
		think_errorerror();
		return -1;
	}

	return r;
}
int think_db_putfield(THINK_DB *db,unsigned int no,const void *value,unsigned int length,int flags)
{
	THINK_DB_FIELD *p;
	//char fieldvalue[256];
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if(no>=db->field_count){
		think_error(0,"[%s]:field no out of range.[no=%d]",__func__);
		return -1;
	}
	p=db->field_array+no;
	//bzero(fieldvalue,sizeof(fieldvalue));
	switch(p->type){
		case THINK_DB_CHAR:
			if(!(flags & THINK_DB_NOT_STRING))
				length=strlen(value);
			if(length>p->length){
				think_error(0,"[%s]:value too long![%d:%s],[type=char],[length=%d],[len=%d]",__func__,no,p->name,p->length,length);
				return -1;
			}
			//memcpy(fieldvalue,value,length);
			memcpy(db->record_buffer+db->offset_array[no],value,length);
			memset(db->record_buffer+db->offset_array[no]+length,0x00,p->length-length);
			break;
		case THINK_DB_SHORT:
			*((THINK_SHORT*)(db->record_buffer + db->offset_array[no])) = think_htob2(*(THINK_SHORT*)value);
			break;
		case THINK_DB_INT:
		case THINK_DB_FLOAT:
			*((THINK_INT*)(db->record_buffer + db->offset_array[no])) = think_htob4(*(THINK_INT*)value);
			break;
		case THINK_DB_LONG:
		case THINK_DB_DOUBLE:
			*((THINK_LONG*)(db->record_buffer + db->offset_array[no])) = think_htob8(*(THINK_LONG*)value);
			break;
		default:
			break;
	};

	return 0;
}
int think_db_putfieldbyname(THINK_DB *db,const char *name,const void *value,unsigned int length,int flags)
{
	THINK_DB_FIELD *p;
	int i;

	for(i=0,p=db->field_array;i<db->field_count;i++,p++){
		if(strcmp(name,p->name)==0)
			break;
	}
	if(i==db->field_count){
		think_error(0,"[%s]:field not exists.[name=%s]",__func__,name);
		return -1;
	}
	if(think_db_putfield(db,i,value,length,flags)<0){
		think_errorerror();
		return -1;
	}

	return 0;
}

int think_db_eof(THINK_DB *db)
{
	if(db->cursor==THINK_DB_EOF)
		return 1;

	return 0;
}

int think_db_getfieldflags(const char *strflags)
{
	int flags=0;

	return flags;
}
