#include "think_error.h"
#include "think_utility.h"
#include "think_conf.h"
#include "think_gl.h"

/* think_gl_attr_get */
int think_gl_attr_get(const char *attrbuf,unsigned short attrlen,unsigned char attrno,void *data,unsigned char size)
{
	const char *p;
	unsigned char len;

	if((p=think_gl_attr_find(attrbuf,attrlen,attrno))==NULL){
		think_errorerror();
		return -1;
	}
	/* no */
	p++;

	/* len */
	len=*p;
	p++;

	/* data */
	if(len>size){
		think_error(0,"[%s]:attr data too long.[attrno=%d][len=%d][size=%d]",__func__,attrno,len,size);
		return -1;
	}
	memcpy(data,p,len);

	return len;
}

/* think_gl_attr_put */
int think_gl_attr_put(char *attrbuf,unsigned short attrsiz,unsigned char attrno,const void *data,unsigned char len)
{
	char *p;

	/* no+len+data */
	if(attrsiz<1+1+len){
		think_error(0,"[%s]:attr buffer too short.[len=%d][attrsiz=%d]",__func__,len,attrsiz);
		return -1;
	}
	p=attrbuf;

	/* attrno */
	memcpy(p,&attrno,1);
	p++;

	/* len */
	memcpy(p,&len,1);
	p++;

	/* data */
	memcpy(p,data,len);

	return 1+1+len;
}

/* think_gl_get */
int think_gl_get(const char *msgbuf,unsigned int msglen,unsigned short no,char *name,char type,unsigned int maxlen,int flags,char *desc,void *data,unsigned int size,int opflags)
{
	THINK_USHORT s;
	THINK_UINT i;
	THINK_ULONG l;
	const char *p;
	THINK_UINT len;
	THINK_USHORT attrlen;

	if((p=think_gl_find(msgbuf,msglen,no))==NULL){
		think_errorerror();
		return -1;
	}

	/* no */
	p+=2;

	/* attr length */
	memcpy(&attrlen,p,2);
	attrlen=think_btoh2(attrlen);
	p+=2;

	/* attr data */
	p+=attrlen;

	/* field length */
	memcpy(&len,p,4);
	len=think_btoh4(len);
	p+=4;

	/* data */
	switch(type){
		case THINK_GL_CHAR:		/* char */
			if(len>maxlen){
				think_error(0,"[%s]:data too long![no=%d][type='%c'][maxlen=%d][len=%d]",__func__,no,type,maxlen,len);
				return -1;
			}
			if(len>size){
				think_error(0,"[%s]:data too long.[no=%d][type='%c'][size=%d][len=%d]",__func__,no,type,size,len);
				return -1;
			}
			memcpy(data,p,len);
			if(!(flags & THINK_GL_BINARY || flags & THINK_GL_RECORD || flags & THINK_GL_RECORDSET) && !(opflags & THINK_GL_NOT_STRING)){
				if(len==size){
					think_error(0,"[%s]:value size equal to len.[no=%d][type='%c'][size=%d][len=%d]",__func__,no,type,size,len);
					return -1;
				}
				*((char *)data+len)='\0';
				if(!(opflags & THINK_GL_NOT_TRIM))
					think_strtrim(data);
				len=strlen(data);
				if(len==0 && opflags&THINK_GL_NOT_NULL){
					think_error(0,"[%s]:field is null.[no=%d]",__func__,no);
					return -1;
				}
			}
			if(opflags & THINK_GL_FIXLEN){
				if(len!=maxlen){
					think_error(0,"[%s]:length not match on fixlen field.[no=%d][len=%d][maxlen=%d]",__func__,no,len,maxlen);
					return -1;
				}
			}
			break;
		case THINK_GL_SHORT:		/* short */
			if(len!=2){
				think_error(0,"[%s]:field length not match.[no=%d][type='%c'][len=%d]",__func__,no,type,len);
				return -1;
			}
			memcpy(&s,p,2);
			if(opflags & THINK_GL_NOT_NULL){
				if(s==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			s=think_btoh2(s);
			memcpy(data,&s,2);
			break;
		case THINK_GL_INT:	/* int */
			if(len!=4){
				think_error(0,"[%s]:field length not match.[no=%d][type='%c'][len=%d]",__func__,no,type,len);
				return -1;
			}
			memcpy(&i,p,4);
			if(opflags & THINK_GL_NOT_NULL){
				if(i==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			i=think_btoh4(i);
			memcpy(data,&i,4);
			break;
		case THINK_GL_LONG:	/* long */
			if(len!=8){
				think_error(0,"[%s]:field length not match.[no=%d][type='%c'][len=%d]",__func__,no,type,len);
				return -1;
			}
			memcpy(&l,p,8);
			if(opflags & THINK_GL_NOT_NULL){
				if(l==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			l=think_btoh8(l);
			memcpy(data,&l,8);
			break;
		case THINK_GL_FLOAT:		/* float */
			if(len!=4){
				think_error(0,"[%s]:field length not match.[no=%d][type='%c'][len=%d]",__func__,no,type,len);
				return -1;
			}
			memcpy(&i,p,4);
			if(opflags & THINK_GL_NOT_NULL){
				if(i==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			i=think_btoh4(i);
			memcpy(data,&i,4);
			break;
		case THINK_GL_DOUBLE:		/* double */
			if(len!=8){
				think_error(0,"[%s]:field length not match.[no=%d][type='%c'][len=%d]",__func__,no,type,len);
				return -1;
			}
			memcpy(&l,p,8);
			if(opflags & THINK_GL_NOT_NULL){
				if(l==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			l=think_btoh8(l);
			memcpy(data,&l,8);
			break;
		default:
			think_error(0,"[%s]:illegal rule![no=%d][type='%c']",__func__,no,type);
			return -1;
	}

	return len;
}

/* think_gl_put */
int think_gl_put(char *msgbuf,unsigned int msgsiz,unsigned short no,const char *name,char type,unsigned int maxlen,int flags,const char *desc,const void *data,unsigned int len,int opflags)
{
	THINK_USHORT s;
	THINK_UINT i;
	THINK_ULONG l;
	char *attrbuf;
	THINK_UINT msglen;
	THINK_USHORT attrlen,attrsize;
	int r;

	/* no+attrlen+attrdata+len+data(2+2+...+4+...) */

	if(msgsiz<8){
		think_error(0,"[%s]:msgbuf too short![no=%d][type='%c'][len=%d][msgsiz=%d]",__func__,no,type,len,msgsiz);
		return -1;
	}
	attrbuf=msgbuf+4;
	attrlen=0;
	attrsize=msgsiz-8;

	/* attr data */
	if(opflags & THINK_GL_DEBUG){
		/* name */
		if((r=think_gl_attr_put(attrbuf+attrlen,attrsize-attrlen,THINK_GL_NAME,name,strlen(name)))<0){
			think_error(0,"[%s]:[no=%d]:%s",__func__,no,__think_errmsg__);
			return -1;
		}
		attrlen+=r;

		/* type */
		if((r=think_gl_attr_put(attrbuf+attrlen,attrsize-attrlen,THINK_GL_TYPE,&type,1))<0){
			think_error(0,"[%s]:[no=%d]:%s",__func__,no,__think_errmsg__);
			return -1;
		}
		attrlen+=r;

		/* maxlen */
		i=maxlen;
		i=think_htob4(i);
		if((r=think_gl_attr_put(attrbuf+attrlen,attrsize-attrlen,THINK_GL_MAXLEN,&i,4))<0){
			think_error(0,"[%s]:[no=%d]:%s",__func__,no,__think_errmsg__);
			return -1;
		}
		attrlen+=r;

		/* flags */
		i=flags;
		i=think_htob4(i);
		if((r=think_gl_attr_put(attrbuf+attrlen,attrsize-attrlen,THINK_GL_FLAGS,&i,4))<0){
			think_error(0,"[%s]:[no=%d]:%s",__func__,no,__think_errmsg__);
			return -1;
		}
		attrlen+=r;

		/* desc */
		if((r=think_gl_attr_put(attrbuf+attrlen,attrsize-attrlen,THINK_GL_DESC,desc,strlen(desc)))<0){
			think_error(0,"[%s]:[no=%d]:%s",__func__,no,__think_errmsg__);
			return -1;
		}
		attrlen+=r;
	}

	/* no */
	s=think_htob2(no);
	memcpy(msgbuf,&s,2);

	/* attr len */
	s=think_htob2(attrlen);
	memcpy(msgbuf+2,&s,2);

	/* data len */

	/* msglen */
	msglen=2+2+attrlen;

	/* data */
	switch(type){
		case THINK_GL_CHAR:		/* char */
			if(len>maxlen){
				think_error(0,"[%s]:data too long![no=%d][type='%c'][maxlen=%d][len=%d]",__func__,no,type,maxlen,len);
				return -1;
			}
			if(msgsiz<msglen+4+len){
				think_error(0,"[%s]:msgbuf too short![msglen=%d][no=%d][type='%c'][msgsiz=%d]len=[%d]",__func__,msglen,no,type,msgsiz,len);
				return -1;
			}

			if(!(flags & THINK_GL_BINARY || flags & THINK_GL_RECORD || flags & THINK_GL_RECORDSET) && !(opflags & THINK_GL_NOT_STRING)){
				len=strlen(data);
				if(len==0 && opflags&THINK_GL_NOT_NULL){
					think_error(0,"[%s]:field is null.[no=%d]",__func__,no);
					return -1;
				}
			}
			if(opflags & THINK_GL_FIXLEN){
				if(len!=maxlen){
					think_error(0,"[%s]:length not match on fixlen field.[no=%d][len=%d][maxlen=%d]",__func__,no,len,maxlen);
					return -1;
				}
			}
			
			/* len */
			i=len;
			i=think_htob4(i);
			memcpy(msgbuf+msglen,&i,4);
			msglen+=4;

			/* data */
			memcpy(msgbuf+msglen,data,len);
			msglen+=len;

			break;
		case THINK_GL_SHORT:		/* short */
			if(msgsiz<msglen+4+2){
				think_error(0,"[%s]:msgbuf too short![msglen=%d][no=%d][type='%c'][msgsiz=%d]",__func__,msglen,no,type,msgsiz);
				return -1;
			}

			/* len */
			i=think_htob4(2);
			memcpy(msgbuf+msglen,&i,4);
			msglen+=4;

			/* data */
			memcpy(&s,data,2);
			if(opflags & THINK_GL_NOT_NULL){
				if(s==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			s=think_htob2(s);
			memcpy(msgbuf+msglen,&s,2);
			msglen+=2;

			break;
		case THINK_GL_INT:		/* int */
			if(msgsiz<msglen+4+4){
				think_error(0,"[%s]:msgbuf too short![msglen=%d][no=%d][type='%c'][msgsiz=%d]",__func__,msglen,no,type,msgsiz);
				return -1;
			}

			/* len */
			i=think_htob4(4);
			memcpy(msgbuf+msglen,&i,4);
			msglen+=4;

			/* data */
			memcpy(&i,data,4);
			if(opflags & THINK_GL_NOT_NULL){
				if(i==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			i=think_htob4(i);
			memcpy(msgbuf+msglen,&i,4);
			msglen+=4;

			break;
		case THINK_GL_LONG:		/* long */
			if(msgsiz<msglen+4+8){
				think_error(0,"[%s]:msgbuf too short![msglen=%d][no=%d][type='%c'][msgsiz=%d]",__func__,msglen,no,type,msgsiz);
				return -1;
			}

			/* len */
			i=think_htob4(8);
			memcpy(msgbuf+msglen,&i,4);
			msglen+=4;

			/* data */
			memcpy(&l,data,8);
			if(opflags & THINK_GL_NOT_NULL){
				if(l==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			l=think_htob8(l);
			memcpy(msgbuf+msglen,&l,8);
			msglen+=8;

			break;
		case THINK_GL_FLOAT:		/* float */
			if(msgsiz<msglen+4+4){
				think_error(0,"[%s]:msgbuf too short![msglen=%d][no=%d][type='%c'][msgsiz=%d]",__func__,msglen,no,type,msgsiz);
				return -1;
			}

			/* len */
			i=think_htob4(4);
			memcpy(msgbuf+msglen,&i,4);
			msglen+=4;

			/* data */
			memcpy(&i,data,4);
			if(opflags & THINK_GL_NOT_NULL){
				if(i==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			i=think_htob4(i);
			memcpy(msgbuf+msglen,&i,4);
			msglen+=4;

			break;
		case THINK_GL_DOUBLE:		/* double */
			if(msgsiz<msglen+4+8){
				think_error(0,"[%s]:msgbuf too short![msglen=%d][no=%d][type='%c'][msgsiz=%d]",__func__,msglen,no,type,msgsiz);
				return -1;
			}

			/* len */
			i=think_htob4(8);
			memcpy(msgbuf+msglen,&i,4);
			msglen+=4;

			/* data */
			memcpy(&l,data,8);
			if(opflags & THINK_GL_NOT_NULL){
				if(l==0){
					think_error(0,"[%s]:field value is 0.[no=%d]",__func__,no);
					return -1;
				}
			}
			l=think_htob8(l);
			memcpy(msgbuf+msglen,&l,8);
			msglen+=8;

			break;
		default:
			think_error(0,"[%s]:illegal rule![no=%d][type='%c']",__func__,no,type);
			return -1;
	}

	return msglen;
}

const char *think_gl_find(const char *msgbuf,unsigned int msglen,unsigned short no)
{
	const char *p,*q;
	THINK_UINT pos;
	THINK_UINT len;
	THINK_USHORT t,attrlen;

	pos=0;
	t=0;
	while(pos!=msglen){
		if(msglen-pos<8){
			think_error(0,"[%s]:message not complete.[no=%d][curno=%d][pos=%d][msglen=%d]",__func__,no,t,pos,msglen);
			return NULL;
		}
		p=msgbuf+pos;

		/* no */
		memcpy(&t,p,2);
		t=think_btoh2(t);
		if(t==no){ /* find ok,check format */
			q=p;

			p+=2;
			pos+=2;

			/* attr len */
			memcpy(&attrlen,p,2);
			attrlen=think_btoh2(attrlen);
			p+=2;
			pos+=2;

			/* attr data */
			if(attrlen>msglen-pos-4){
				think_error(0,"[%s]:attr not complete.[no=%d][curno=%d][pos=%d][msglen=%d][attrlen=%d]",__func__,no,t,pos,msglen,attrlen);
				return NULL;
			}
			p+=attrlen;
			pos+=attrlen;

			/* len */
			memcpy(&len,p,4);
			len=think_btoh4(len);
			p+=4;
			pos+=4;

			/* data */
			if(len>msglen-pos){
				think_error(0,"[%s]:data not complete.[no=%d][curno=%d][pos=%d][msglen=%d][len=%d]",__func__,no,t,pos,msglen,len);
				return NULL;
			}

			return q;
		}
		p+=2;
		pos+=2;

		/* attr len */
		memcpy(&attrlen,p,2);
		attrlen=think_btoh2(attrlen);
		p+=2;
		pos+=2;

		/* attr data */
		if(attrlen>msglen-pos-4){
			think_error(0,"[%s]:attr not complete.[no=%d][curno=%d][pos=%d][msglen=%d][attrlen=%d]",__func__,no,t,pos,msglen,attrlen);
			return NULL;
		}
		p+=attrlen;
		pos+=attrlen;

		/* len */
		memcpy(&len,p,4);
		len=think_btoh4(len);
		p+=4;
		pos+=4;

		/* data */
		if(len>msglen-pos){
			think_error(0,"[%s]:data not complete.[no=%d][curno=%d][pos=%d][msglen=%d][len=%d]",__func__,no,t,pos,msglen,len);
			return NULL;
		}
		p+=len;
		pos+=len;
	}
	think_error(THINK_ERRNO_GL_FIELD_NOT_EXIST,"[%s]:field not exists![no=%d]",__func__,no);

	return NULL;
}

/* think_gl_del */
int think_gl_del(char *msgbuf,unsigned int msglen,unsigned short no)
{
	const char *p,*q;
	THINK_UINT len;
	THINK_USHORT attrlen;

	if((p=think_gl_find(msgbuf,msglen,no))==NULL){
		think_errorerror();
		return -1;
	}

	q=p;

	/* no */
	p+=2;

	/* attr length */
	memcpy(&attrlen,p,2);
	attrlen=think_btoh2(attrlen);
	p+=2;

	/* attr data */
	p+=attrlen;

	/* field length */
	memcpy(&len,p,4);
	len=think_btoh4(len);
	p+=4;

	/* data */
	p+=len;

	memmove((char *)q,p,msglen-(p-msgbuf));

	return p-q;
}

/* think_gl_gets */
int think_gl_gets(const char *msgbuf,unsigned int msglen,const THINK_GL_FIELDRULE *fieldrulearray,unsigned int fieldrulecount,int opflags)
{
	int i,r;
	const THINK_GL_FIELDRULE *p;

	for(i=0,p=fieldrulearray;i<fieldrulecount;i++,p++){
		if((r=think_gl_get(msgbuf,msglen,p->no,NULL,p->type,p->maxlen,p->flags,NULL,p->data,p->size,p->opflags|opflags))<0){
			if((p->opflags|opflags) & THINK_GL_IGNORE){
				if(p->exist)
					*p->exist=0;
				continue;
			}
			think_errorerror();
			return -1;
		}
		if(p->len)
			*p->len=r;
		if(p->exist)
			*p->exist=1;
	}

	return 0;
}

/* think_gl_puts */
int think_gl_puts(char *msgbuf,unsigned int msgsiz,const THINK_GL_FIELDRULE *fieldrulearray,unsigned int fieldrulecount,int opflags)
{
	int i,r;
	const THINK_GL_FIELDRULE *p;
	unsigned int msglen,len;

	msglen=0;
	for(msglen=0,i=0,p=fieldrulearray;i<fieldrulecount;i++,p++,msglen+=r){
		len=p->len?*p->len:0;
		if((r=think_gl_put(msgbuf+msglen,msgsiz-msglen,p->no,p->name,p->type,p->maxlen,p->flags,p->desc,p->data,len,p->opflags|opflags))<0){
			think_errorerror();
			return -1;
		}
	}

	return msglen;
}

/* think_gl_autoget */
int think_gl_autoget(const char *msgbuf,unsigned int msglen,unsigned short no,void *data,unsigned int size,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags)
{
	int i,r;
	const THINK_GL_RULE *p;
	unsigned int len;

	for(i=0,p=rulearray;i<rulecount;i++,p++){
		if(p->no==no)
			break;
	}
	if(i==rulecount){
		think_error(0,"[%s]:rule not exists.[no=%d]",__func__,no);
		return -1;
	}
	if((r=think_gl_get(msgbuf,msglen,p->no,NULL,p->type,p->maxlen,p->flags,NULL,data,size,opflags))<0){
		think_errorerror();
		return -1;
	}
	len=r;

	return len;
}

/* think_gl_autoput */
int think_gl_autoput(char *msgbuf,unsigned int msgsiz,unsigned short no,const void *data,unsigned int len,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags)
{
	int i,r;
	const THINK_GL_RULE *p;
	unsigned int msglen;

	for(i=0,p=rulearray;i<rulecount;i++,p++){
		if(p->no==no)
			break;
	}
	if(i==rulecount){
		think_error(0,"[%s]:rule not exists.[no=%d]",__func__,no);
		return -1;
	}
	if((r=think_gl_put(msgbuf,msgsiz,p->no,p->name,p->type,p->maxlen,p->flags,p->desc,data,len,opflags))<0){
		think_errorerror();
		return -1;
	}
	msglen=r;

	return msglen;
}

/* think_gl_autogets */
int think_gl_autogets(const char *msgbuf,unsigned int msglen,const THINK_GL_FIELD *fieldarray,unsigned int fieldcount,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags)
{
	int i,j,r;
	const THINK_GL_FIELD *p;
	const THINK_GL_RULE *q;

	for(i=0,p=fieldarray;i<fieldcount;i++,p++){
		for(j=0,q=rulearray;j<rulecount;j++,q++){
			if(q->no==p->no)
				break;
		}
		if(j==rulecount){
			think_error(0,"[%s]:rule not exists.[no=%d]",__func__,p->no);
			return -1;
		}
		if((r=think_gl_get(msgbuf,msglen,p->no,NULL,q->type,q->maxlen,q->flags,NULL,p->data,p->size,p->opflags|opflags))<0){
			if((p->opflags|opflags) & THINK_GL_IGNORE){
				if(p->exist)
					*p->exist=0;
				continue;
			}
			think_errorerror();
			return -1;
		}
		if(p->len)
			*p->len=r;
		if(p->exist)
			*p->exist=1;
	}

	return 0;
}

/* think_gl_autoputs */
int think_gl_autoputs(char *msgbuf,unsigned int msgsiz,const THINK_GL_FIELD *fieldarray,unsigned int fieldcount,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags)
{
	int i,j,r;
	const THINK_GL_FIELD *p;
	const THINK_GL_RULE *q;
	unsigned int msglen,len;

	msglen=0;
	for(msglen=0,i=0,p=fieldarray;i<fieldcount;i++,p++,msglen+=r){
		for(j=0,q=rulearray;j<rulecount;j++,q++){
			if(q->no==p->no)
				break;
		}
		if(j==rulecount){
			think_error(0,"[%s]:rule not exists.[no=%d]",__func__,p->no);
			return -1;
		}
		len=p->len?*p->len:0;
		if((r=think_gl_put(msgbuf+msglen,msgsiz-msglen,p->no,q->name,q->type,q->maxlen,q->flags,q->desc,p->data,len,p->opflags|opflags))<0){
			think_errorerror();
			return -1;
		}
	}

	return msglen;
}

/* think_gl_record_get */
int think_gl_record_get(const char *msgbuf,unsigned int msglen,char *recbuf,unsigned int recsiz)
{
	THINK_UINT reclen;

	if(msglen<4){
		think_error(0,"[%s]:message not complete.[msglen=%d]",__func__,msglen);
		return -1;
	}
	memcpy(&reclen,msgbuf,4);
	reclen=think_btoh4(reclen);
	if(reclen>msglen-4){
		think_error(0,"[%s]:data not complete.[msglen=%d][reclen=%d]",__func__,msglen,reclen);
		return -1;
	}
	if(reclen>recsiz){
		think_error(0,"[%s]:record too long.[recsiz=%d][reclen=%d]",__func__,recsiz,reclen);
		return -1;
	}
	memcpy(recbuf,msgbuf+4,reclen);

	return reclen;
}
/* think_gl_record_put */
int think_gl_record_put(char *msgbuf,unsigned int msgsiz,const char *recbuf,unsigned int reclen)
{
	THINK_UINT t;

	if(reclen+4>msgsiz){
		think_error(0,"[%s]:msgbuf too short.[reclen=%d][msgsiz=%d]",__func__,reclen,msgsiz);
		return -1;
	}

	/* record length */
	t=reclen;
	t=think_htob4(t);
	memcpy(msgbuf,&t,4);

	/* record data */
	memcpy(msgbuf+4,recbuf,reclen);

	return reclen+4;
}

/* think_gl_record_fetch */
int think_gl_record_fetch(const char *msgbuf,unsigned int msglen,unsigned int recno,char *recbuf,unsigned int recsiz)
{
	THINK_UINT reclen,pos;
	int n;

	n=0;
	pos=0;
	while(pos!=msglen){
		if(msglen-pos<4){
			think_error(0,"[%s]:message not complete.[recno=%d][curno=%d][pos=%d][msglen=%d]",__func__,recno,n,pos,msglen);
			return -1;
		}

		/* record length */
		memcpy(&reclen,msgbuf+pos,4);
		reclen=think_btoh4(reclen);
		pos+=4;

		/* record data */
		if(reclen>msglen-pos){
			think_error(0,"[%s]:data not complete.[recno=%d][curno=%d][pos=%d][msglen=%d][reclen=%d]",__func__,recno,n,pos,msglen,reclen);
			return -1;
		}
		if(n==recno){
			if(reclen>recsiz){
				think_error(0,"[%s]:record too long.[recno=%d][curno=%d][recsiz=%d][reclen=%d]",__func__,recno,n,recsiz,reclen);
				return -1;
			}
			memcpy(recbuf,msgbuf+pos,reclen);
			return reclen;
		}
		pos+=reclen;

		n++;
	}
	think_error(0,"[%s]:record no exists.[recno=%d][totno=%d]",__func__,recno,n);

	return -1;
}

/* think_gl_record_stat */
int think_gl_record_stat(const char *msgbuf,unsigned int msglen)
{
	THINK_UINT reclen,pos;
	int n;

	n=0;
	pos=0;
	while(pos!=msglen){
		if(msglen-pos<4){
			think_error(0,"[%s]:message not complete.[curno=%d][pos=%d][msglen=%d]",__func__,n,pos,msglen);
			return -1;
		}

		/* record len */
		memcpy(&reclen,msgbuf+pos,4);
		reclen=think_btoh4(reclen);
		pos+=4;

		/* record data */
		if(reclen>msglen-pos){
			think_error(0,"[%s]:data not complete.[curno=%d][pos=%d][msglen=%d][reclen=%d]",__func__,n,pos,msglen,reclen);
			return -1;
		}
		pos+=reclen;

		n++;
	}

	return n;
}

/* think_gl_loadrules */
int think_gl_loadrules(const char *file,THINK_GL_RULE **rulearray)
{
	THINK_CONF *conf;
	THINK_GL_RULE *p;
	int i;
	char strflags[1024];

	if((conf=think_loadconf(file,'|'))==NULL){
		think_errorerror();
		return -1;
	}
	if(conf->hcount==0)
		return 0;
	*rulearray=malloc(sizeof(THINK_GL_RULE)*conf->hcount);
	for(i=0,p=*rulearray;i<conf->hcount;i++,p++){
		think_getconf(conf,i,0,'S',&p->no,sizeof(p->no),0);
		think_getconf(conf,i,1,'C',p->name,sizeof(p->name),0);
		think_getconf(conf,i,2,'C',&p->type,1,THINK_CONF_NOT_STRING);
		think_getconf(conf,i,3,'I',&p->maxlen,sizeof(p->maxlen),0);
		think_getconf(conf,i,4,'C',strflags,sizeof(strflags),0);
		p->flags=think_gl_getflags(strflags);
		think_getconf(conf,i,5,'C',p->desc,sizeof(p->desc),0);
	}
	think_freeconf(conf);

	return i;
}

/* think_gl_getflags */
int think_gl_getflags(const char *strflags)
{
	int flags;
	
	flags=0;
	if(strstr(strflags,"binary"))
		flags|=THINK_GL_BINARY;
	if(strstr(strflags,"unsigned"))
		flags|=THINK_GL_UNSIGNED;
	if(strstr(strflags,"recordset"))
		flags|=THINK_GL_RECORDSET;
	else if(strstr(strflags,"record"))
		flags|=THINK_GL_RECORD;
	if(strstr(strflags,"ignore"))
		flags|=THINK_GL_IGNORE;
	if(strstr(strflags,"notstring"))
		flags|=THINK_GL_NOT_STRING;
	if(strstr(strflags,"nottrim"))
		flags|=THINK_GL_NOT_TRIM;
	if(strstr(strflags,"notnull"))
		flags|=THINK_GL_NOT_NULL;
	if(strstr(strflags,"fixlen"))
		flags|=THINK_GL_FIXLEN;
	
	return flags;
}

const char *think_gl_attr_find(const char *attrbuf,unsigned short attrlen,unsigned char attrno)
{
	const char *p,*q;
	unsigned short pos;
	unsigned char len;
	unsigned char t;

	pos=0;
	while(pos!=attrlen){
		if(attrlen-pos<2){
			think_error(0,"[%s]:attr not complete.[attrno=%d][curattrno=%d][pos=%d][attren=%d]",__func__,attrno,t,pos,attrlen);
			return NULL;
		}
		p=attrbuf+pos;

		/* attr no */
		memcpy(&t,p,1);
		if(t==attrno){ /* find ok */
			q=p;

			p++;
			pos++;

			/* len */
			memcpy(&len,p,1);
			p++;
			pos++;

			if(len>attrlen-pos){
				think_error(0,"[%s]:attr data not complete.[attrno=%d][curattrno=%d][pos=%d][attrlen=%d][len=%d]",__func__,attrno,t,pos,attrlen,len);
				return NULL;
			}

			return q;
		}
		p++;
		pos++;

		/* len */
		memcpy(&len,p,1);
		p++;
		pos++;

		if(len>attrlen-pos){
			think_error(0,"[%s]:attr data not complete.[attrno=%d][curattrno=%d][pos=%d][attrlen=%d][len=%d]",__func__,attrno,t,pos,attrlen,len);
			return NULL;
		}

		pos+=len;
	}
	think_error(THINK_ERRNO_GL_ATTR_NOT_EXIST,"[%s]:attr not exists![attrno=%d]",__func__,attrno);

	return NULL;
}
