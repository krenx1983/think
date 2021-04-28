#include "think_error.h"
#include "think_utility.h"
#include "think_file.h"
#include "think_gl.h"
#include "think_log.h"

static THINK_GL_RULE rulearray[]={
	{0	,"type"		,'C'	,2	,0					,"message type"},
	{1	,"name"		,'C'	,127,0					,"log name"},
	{2	,"oflags"	,'I'	,0	,THINK_GL_UNSIGNED	,"open flags"},
	{3	,"ip"		,'C'	,15	,0					,"IP address"},
	{4	,"pid"		,'I'	,0	,THINK_GL_UNSIGNED	,"process id"},
	{5	,"tid"		,'I'	,0	,THINK_GL_UNSIGNED	,"thread id"},
	{6	,"time"		,'C'	,19	,0					,"log time"},
	{7	,"level"	,'I'	,0	,THINK_GL_UNSIGNED	,"log level"},
	{8	,"desc"		,'C'	,31	,0					,"log desc"},
	{9	,"info"		,'C'	,511,0					,"log info"},
	{10	,"endflag"	,'C'	,1	,0					,"end flag"},
	{126,"respcode"	,'C'	,2	,0					,"response code"},
	{127,"resptext"	,'C'	,127,0					,"response text"}
};
static int rulecount=sizeof(rulearray)/sizeof(THINK_GL_RULE);

THINK_LOG *think_logopen(const char *addr,const char *name,int flags)
{
	THINK_NET *net;
	char msgbuf[1024],respcode[3],resptext[128];
	char path[256],subpath[256],fullpath[256],filename[256];
	char fullname[256],ip[16];
	unsigned short port;
	THINK_LOG *lp;
	THINK_DB *db;
	int msglen,r,oflags;
	int glflags=0;
	THINK_DB_FIELD fieldarray[6]={
		{"IP",'C',15,0},
		{"PID",'I',0,0},
		{"Time",'C',19,0},
		{"Level",'I',0,0},
		{"Desc",'C',31,0},
		{"Info",'C',511,0}
	};

	/* gl debug flag */
	if(flags & THINK_LOG_G)
		glflags|=THINK_GL_DEBUG;

	/* lp */
	if((lp=malloc(sizeof(THINK_LOG)))==NULL){
		think_error(0,"[%s]:malloc think_error![%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}
	bzero(lp,sizeof(THINK_LOG));
	lp->net=NULL;	/* net */
	lp->db=NULL;	/* local */
	snprintf(lp->addr,sizeof(lp->addr),"%s",addr);	/* addr */
	snprintf(lp->name,sizeof(lp->name),"%s",name);	/* name */
	lp->flags=flags;	/* flags */
	lp->pid=getpid();	/* pid */
	if((lp->flags & THINK_LOG_MASK_LEVEL)==0)
		lp->flags|=THINK_LOG_NORMAL;	/* default level */

	if(flags & THINK_LOG_LOCAL){	/* local */
		/* get path */
		strcpy(path,lp->addr);
		if(strlen(path)>1 && path[strlen(path)-1]=='/')
			path[strlen(path)-1]='\0';	/* delete last separate char '/' */
		if(strlen(path)==0)
			strcpy(path,".");	/* current dir */

		/* get subpath & filename */
		bzero(subpath,sizeof(subpath));
		bzero(filename,sizeof(filename));
		if(lp->name[0]=='/'){
			think_error(0,"[%s]:name illegal.[%s]",__func__,name);
			free(lp);
			return NULL;
		}
		if((r=think_chrfind(lp->name,'/',-1))<0){
			strcpy(subpath,"");
			strcpy(filename,lp->name);
		}else{
			memcpy(subpath,lp->name,r);
			strcpy(filename,lp->name+r+1);
		}

		/* get fullpath */
		if(strlen(subpath)==0)
			strcpy(fullpath,path);
		else
			sprintf(fullpath,"%s/%s",path,subpath);
		if(think_automkdir(fullpath,0777)<0){	/* auto mkdir */
			think_errorerror();
			free(lp);
			return NULL;
		}

		/* get full file */
		sprintf(fullname,"%s/log%s.db",fullpath,filename);

		/* db */
		oflags=THINK_DB_CREAT|THINK_DB_RDWR|THINK_DB_APPEND|THINK_DB_SYNC;
		if(lp->flags & THINK_LOG_TRUNC)
			oflags|=THINK_DB_TRUNC;
		if(lp->flags & THINK_LOG_EXCL)
			oflags|=THINK_DB_EXCL;
		if((db=think_db_open(fullname,oflags,fieldarray,6))==NULL){
			think_errorerror();
			free(lp);
			return NULL;
		}
		lp->db=db;
	}else{	/* NET */
		/* init */
		msglen=0;

		/* 0 msgtype */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,0,"10",0,rulearray,rulecount,glflags);

		/* 1 name */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,1,name,0,rulearray,rulecount,glflags);

		/* 2 flags */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,2,&flags,0,rulearray,rulecount,glflags);

		/* 4 pid */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,4,&lp->pid,0,rulearray,rulecount,glflags);

		/* ip:port */
		strcpy(ip,"127.0.0.1");
		port=10000;
		if(strlen(lp->addr)==0)
			strcpy(lp->addr,"127.0.0.1:10000");
		think_strgetfield(lp->addr,':',0,'C',ip,sizeof(ip),0);
		think_strgetfield(lp->addr,':',1,'S',&port,0,0);

		/* connect */
		if((net=think_netconnect(ip,port))==NULL){
			think_errorerror();
			free(lp);
			return NULL;
		}

		/* send package */
		if(think_netsendmsg(net,msgbuf,msglen)<0){
			think_errorerror();
			think_netclose(net);
			free(lp);
			return NULL;
		}
		if((msglen=think_netrecvmsg(net,msgbuf,sizeof(msgbuf)))<0){
			think_errorerror();
			think_netclose(net);
			free(lp);
			return NULL;
		}
		bzero(respcode,sizeof(respcode));
		if(think_gl_autoget(msgbuf,msglen,126,respcode,sizeof(respcode),rulearray,rulecount,glflags)<0){
			think_errorerror();
			think_netclose(net);
			free(lp);
			return NULL;
		}
		if(think_gl_autoget(msgbuf,msglen,127,resptext,sizeof(resptext),rulearray,rulecount,glflags)<0){
			think_errorerror();
			think_netclose(net);
			free(lp);
			return NULL;
		}
		if(strcmp(respcode,"00")!=0){
			think_error(0,"[%s]:[%s:%s]",__func__,respcode,resptext);
			think_netclose(net);
			free(lp);
			return NULL;
		}
		lp->net=net;
	}

	return lp;
}

int think_logsend(THINK_LOG *lp,int flags,const char *desc,const char *fmt,...)
{
	char msgbuf[1024];
	int msglen,r;
	va_list args;
	char logtime[20],logdesc[32],loginfo[512];
	int loglevel,pos;
	char prompt[128];
	struct tm *t;
	time_t tt;
	int glflags=0;

	if(!lp)
		return -1;

	if(lp->flags & THINK_LOG_G)
		glflags|=THINK_GL_DEBUG;

	/* time "yyyy-mm-dd hh:mm:ss" */
	tt=time(NULL);
	t=localtime(&tt);
	sprintf(logtime,"%04d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

	/* level */
	loglevel=flags & THINK_LOG_MASK_LEVEL;
	if(loglevel==0)
		loglevel=THINK_LOG_NORMAL;	/* default level */
	if(loglevel<(lp->flags & THINK_LOG_MASK_LEVEL))
		return 0;	/* level lower than settings */

	/* desc */
	bzero(logdesc,sizeof(logdesc));
	snprintf(logdesc,sizeof(logdesc),"%s",desc);

	/* info */
	va_start(args,fmt);
	vsnprintf(loginfo,sizeof(loginfo),fmt,args);
	va_end(args);

	/* print */
	if(lp->flags & THINK_LOG_PRINT){
		sprintf(prompt,"[%s]-[%d]-[%s]-",logtime,loglevel,logdesc);
		fwrite(prompt,1,strlen(prompt),stdout);
		pos=0;
		while(pos!=strlen(loginfo)){
			r=think_chrfind(loginfo+pos,'\n',0);
			if(r<0 || (pos+r+1)==strlen(loginfo)){
				fwrite(loginfo+pos,1,strlen(loginfo)-pos,stdout);
				break;
			}
			fwrite(loginfo+pos,1,r+1,stdout);
			fwrite(prompt,1,strlen(prompt),stdout);
			pos+=r+1;
		}
		fwrite("\n",1,1,stdout);
	}

	if(!(lp->flags & THINK_LOG_LOCAL)){	/* NET */
		/* init */
		msglen=0;

		/* 1 msgtype */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,0,"20",0,rulearray,rulecount,glflags);

		/* 7 time */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,6,logtime,0,rulearray,rulecount,glflags);

		/* 8 level */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,7,&loglevel,0,rulearray,rulecount,glflags);

		/* 9 desc */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,8,logdesc,0,rulearray,rulecount,glflags);

		/* 10 info */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,9,loginfo,0,rulearray,rulecount,glflags);

		/* send package */
		if(think_netsendmsg(lp->net,msgbuf,msglen)<0){
			think_errorerror();
			return -1;
		}
	}else{	/* local */
		think_db_new(lp->db);
		think_db_putfield(lp->db,1,&lp->pid,sizeof(lp->pid),0);
		think_db_putfield(lp->db,2,logtime,strlen(logtime),0);
		think_db_putfield(lp->db,3,&loglevel,sizeof(loglevel),0);
		think_db_putfield(lp->db,4,logdesc,strlen(logdesc),0);
		think_db_putfield(lp->db,5,loginfo,strlen(loginfo),0);
		if(think_db_insert(lp->db)<0){
			think_errorerror();
			return -1;
		}
	}

	return 0;
}
int think_logclose(THINK_LOG *lp)
{
	if(!lp)
		return -1;
	
	if(lp->flags & THINK_LOG_LOCAL){
		/* local */
		think_db_close(lp->db);
	}else{	/* net */
		think_netclose(lp->net);
	}
	free(lp);

	return 0;
}

int think_loggetflags(const char *strflags)
{
	int flags;
	
	flags=0;
	if(strstr(strflags,"debug"))
		flags|=THINK_LOG_DEBUG;
	if(strstr(strflags,"normal"))
		flags|=THINK_LOG_NORMAL;
	if(strstr(strflags,"info"))
		flags|=THINK_LOG_INFO;
	if(strstr(strflags,"key"))
		flags|=THINK_LOG_KEY;
	if(strstr(strflags,"warn"))
		flags|=THINK_LOG_WARN;
	if(strstr(strflags,"error"))
		flags|=THINK_LOG_ERROR;
	if(strstr(strflags,"fatal"))
		flags|=THINK_LOG_FATAL;
	if(strstr(strflags,"print"))
		flags|=THINK_LOG_PRINT;
	if(strstr(strflags,"trunc"))
		flags|=THINK_LOG_TRUNC;
	if(strstr(strflags,"excl"))
		flags|=THINK_LOG_EXCL;
	if(strstr(strflags,"local"))
		flags|=THINK_LOG_LOCAL;
	
	return flags;
}

