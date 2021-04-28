#include "logserver.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: logserver [-G] [-p cfgpath] [-c cfgfile]";
	unsigned short lsnport;
	char lsnip[16];
	THINK_NET *net;
	THINK_CFG *cfg;
	int G_Exists,i;
	THINK_OPTARRAY optarray[3]={
		{'G','C',NULL,0,0,&G_Exists},
		{'p','C',cfgpath,sizeof(cfgpath),THINK_OPT_ARG,NULL},
		{'c','C',cfgfile,sizeof(cfgfile),THINK_OPT_ARG|THINK_OPT_NOT_NULL,NULL}
	};
	THINK_CFGARRAY cfgarray[5]={
		{"","G",'C',NULL,0,0,&G_Exists},
		{"","lsnaddr",'C',lsn_addr,sizeof(lsn_addr),THINK_CFG_NOT_NULL,NULL},
		{"log","name",'C',log_name,sizeof(log_name),0,NULL},
		{"log","flags",'C',log_flags,sizeof(log_flags),0,NULL},
		{"log","path",'C',log_path,sizeof(log_path),0,NULL}
	};

#ifdef __THINK_UNIX__
	signal(SIGPIPE,SIG_IGN);
#endif

	if(think_getopts(argc,argv,optarray,3)<0){
		think_showerror();
		puts(usage);
		return -1;
	}
	if((cfg=think_loadcfg(cfgfile,cfgpath))==NULL){
		think_showerror();
		return -1;
	}
	if(think_getcfgs(cfg,cfgarray,5)<0){
		think_showerror();
		return -1;
	}
	think_freecfg(cfg);
	if(G_Exists)
		glflags|=THINK_GL_DEBUG;

	/*logpath*/
	if(strlen(log_path)>1 && log_path[strlen(log_path)-1]=='/')
		log_path[strlen(log_path)-1]='\0';	/* delete last separate char '/' */
	if(strlen(log_path)==0)
		strcpy(log_path,".");

	/* net start */
	think_netstart();

	/*listening...*/
	think_strgetfield(lsn_addr,':',0,'C',lsnip,sizeof(lsnip),0);
	think_strgetfield(lsn_addr,':',1,'S',&lsnport,0,0);
	if((net=think_netlisten(lsnip,lsnport))==NULL){
		think_showerror();
		return -1;
	}
	netcenter=think_netcenter_new();
	think_netcenter_register(netcenter,net,acceptnewclient,net);
	if((lp=think_logopen(log_path,log_name,THINK_LOG_LOCAL|think_loggetflags(log_flags)))==NULL){
		think_showerror();
		return -1;
	}
	think_logsend(lp,0,"system","idle ...");
	while(1){
		if(think_netcenter_idle(netcenter,-1)<0){
			think_logsend(lp,THINK_LOG_FATAL,"network","%s",__think_errmsg__);
			think_showerror();
			break;
		}
	}

	/* net stop */
	think_netstop();

	return 0;
}
void acceptnewclient(THINK_NETCENTER_HANDLE handle)
{
	THINK_NET *lsnnet;
	THINK_NET *client;

	lsnnet=handle;
	if((client=think_netaccept(lsnnet))==NULL){
		think_logsend(lp,THINK_LOG_WARN,"network","%s",__think_errmsg__);
		return;
	}
	think_logsend(lp,THINK_LOG_INFO,"network","connected by %s:%d",client->ip,client->port);
	cliunitadd(client);
}
void clientnethandler(THINK_NETCENTER_HANDLE handle)
{
	char msgbuf[1024];
	int msglen;
	int r;
	THINK_NETCENTER_NET *centernet;
	struct cliunit_t *cliunit;

	cliunit=handle;
	centernet=cliunit->centernet;
	think_netcenter_recvto(centernet);
	/* check msg */
	while((r=think_netcenter_ismsgok(centernet))!=0){
		if(r<0){
			think_logsend(lp,THINK_LOG_WARN,"network","%s",__think_errmsg__);
			cliunitdel(centernet);
			return;
		}
		if((r=think_netcenter_recvmsg(centernet,msgbuf,sizeof(msgbuf)))<0){
			think_logsend(lp,THINK_LOG_WARN,"network","%s",__think_errmsg__);
			cliunitdel(centernet);
			return;
		}
		msglen=r;
		/* process msg */
		if(clientmsgprocess(centernet,msgbuf,msglen)<0){
			cliunitdel(centernet);
			return;
		}
	}
}
int clientmsgprocess(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen)
{
	char msgtype[3];

	bzero(msgtype,sizeof(msgtype));
	if(think_gl_autoget(msgbuf,msglen,0,msgtype,sizeof(msgtype),rulearray,rulecount,glflags)<0){
		think_logsend(lp,THINK_LOG_ERROR,"business","%s",__think_errmsg__);
		return -1;
	}
	if(strcmp(msgtype,"10")==0){
		if(_logopen(centernet,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"20")==0){
		if(_logsend(centernet,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"30")==0 || strcmp(msgtype,"40")==0){
		if(_logview(centernet,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"60")==0){
		if(_logclose(centernet,msgbuf,msglen)<0)
			return -1;
	}else{
		think_error(0,"[%s]:unsupported msgtype[%s]",__func__,msgtype);
		think_logsend(lp,THINK_LOG_ERROR,"business","%s",__think_errmsg__);
		return -1;
	}

	return 0;
}
int _logopen(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen)
{
	char name[128];
	THINK_DB *db;
	struct logunit_t *logp;
	struct cliunit_t *clip;
	struct viewlist_t *viewlistp;
	int r,oflags,flags,pid;
	char subpath[256],fullpath[256],filename[256],file[256];
	char msgbufout[1024],respcode[3],resptext[128];
	int msglenout;

	think_logsend(lp,THINK_LOG_INFO,"system","logopen ...");
	/* 1 name */
	bzero(name,sizeof(name));
	if(think_gl_autoget(msgbuf,msglen,1,name,sizeof(name),rulearray,rulecount,glflags)<0){
		think_logsend(lp,THINK_LOG_ERROR,"protocol","%s",__think_errmsg__);
		return -1;
	}
	think_logsend(lp,THINK_LOG_DEBUG,"business","logname=%s",name);
	if(strcmp(name,log_name)==0){
		think_logsend(lp,THINK_LOG_ERROR,"business","open on this log is forbiddened!");
		return -1;
	}

	/* 2 flag */
	flags=0;
	if(think_gl_autoget(msgbuf,msglen,2,&flags,0,rulearray,rulecount,glflags)<0){
		think_logsend(lp,THINK_LOG_ERROR,"protocol","%s",__think_errmsg__);
		return -1;
	}
	/* 4 pid */
	pid=0;
	if(think_gl_autoget(msgbuf,msglen,4,&pid,0,rulearray,rulecount,glflags)<0){
		think_logsend(lp,THINK_LOG_ERROR,"protocol","%s",__think_errmsg__);
		return -1;
	}
	if((logp=logunitsearch(name))==NULL){
		think_logsend(lp,THINK_LOG_DEBUG,"system","this is a new log!");
		/* get subpath & filename */
		bzero(subpath,sizeof(subpath));
		bzero(filename,sizeof(filename));
		if(name[0]=='/'){
			think_logsend(lp,THINK_LOG_ERROR,"business","name illegal.");
			return -1;
		}
		if((r=think_chrfind(name,'/',-1))<0){
			strcpy(subpath,"");
			strcpy(filename,name);
		}else{
			memcpy(subpath,name,r);
			strcpy(filename,name+r+1);
		}

		/* get fullpath */
		if(strlen(subpath)==0)
			strcpy(fullpath,log_path);
		else
			sprintf(fullpath,"%s/%s",log_path,subpath);
		think_logsend(lp,THINK_LOG_DEBUG,"system","creating dir [%s] ...",fullpath);
		if(think_automkdir(fullpath,0777)<0){	/* auto mkdir */
			think_logsend(lp,THINK_LOG_DEBUG,"system","	[Failed]\n%s",__think_errmsg__);
			return -1;
		}
		think_logsend(lp,THINK_LOG_DEBUG,"system","	[OK]");

		/* get full file */
		sprintf(file,"%s/log%s.db",fullpath,filename);

		/* db */
		oflags=THINK_DB_CREAT|THINK_DB_RDWR|THINK_DB_APPEND|THINK_DB_SYNC;
		if(flags & THINK_LOG_TRUNC)
			oflags|=THINK_DB_TRUNC;
		if(flags & THINK_LOG_EXCL)
			oflags|=THINK_DB_EXCL;
		if((db=think_db_open(file,oflags,fieldarray,6))==NULL){
			think_logsend(lp,THINK_LOG_ERROR,"system","%s",__think_errmsg__);
			return -1;
		}
		logunitadd(name,db,oflags,file);
		logp=logunitsearch(name);
		/* go though clilist for add to logunit's viewers */
		think_logsend(lp,THINK_LOG_NORMAL,"business","add viewers waiting to this log ...");
		clip=clilist;
		do{
			if(clip->svflag!='1'){	/* not viewer */
				clip=clip->next;
				continue;
			}
			viewlistp=clip->viewlist;
			do{
				if(strcmp(viewlistp->name,name)==0){
					cliplistadd(&logp->viewlist,clip,viewlistp);
					viewlistp->logunit=logp;
					think_logsend(lp,THINK_LOG_NORMAL,"business","%s:%d added",clip->centernet->net->ip,clip->centernet->net->port);
				}
				viewlistp=viewlistp->next;
			}while(viewlistp!=clip->viewlist);
			clip=clip->next;
		}while(clip!=clilist);
		think_logsend(lp,THINK_LOG_NORMAL,"business","viewers added ok!");
	}
	clip=cliunitsearch(centernet);
	clip->logunit=logp;
	clip->svflag='0';	/* sender */
	clip->pid=pid;
	cliplistadd(&logp->clilist,clip,NULL);	/* add to logunit's clilist */
	think_logsend(lp,THINK_LOG_DEBUG,"business","added to sender list.");

	/* resp */
	strcpy(respcode,"00");
	strcpy(resptext,"succeed!");
	msglenout=0;
	msglenout+=think_gl_autoput(msgbufout,sizeof(msgbufout),0,"11",0,rulearray,rulecount,THINK_GL_NOT_TRIM|glflags);
	msglenout+=think_gl_autoput(msgbufout+msglenout,sizeof(msgbufout)-msglenout,126,respcode,0,rulearray,rulecount,glflags);
	msglenout+=think_gl_autoput(msgbufout+msglenout,sizeof(msgbufout)-msglenout,127,resptext,0,rulearray,rulecount,glflags);
	if(think_netcenter_sendmsg(centernet,msgbufout,msglenout)<0){
		think_logsend(lp,THINK_LOG_ERROR,"net","pkgsend error![%s]",__think_errmsg__);
		return -1;
	}


	return 0;
}
int _logsend(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen)
{
	struct cliunit_t *p;
	THINK_DB *db;
	char logbuf[1024];
	int loglen;
	struct cliplist_t *cliplistp;
	char logtime[20],logdesc[32],loginfo[512];
	int loglevel;

	think_logsend(lp,THINK_LOG_INFO,"system","think_logsend ...");
	memcpy(logbuf,msgbuf,msglen);
	loglen=msglen;
	p=cliunitsearch(centernet);
	if(p->svflag!='0'){
		think_logsend(lp,THINK_LOG_ERROR,"business","this is not a sender!");
		return -1;
	}
	db=p->logunit->db;

	/* 6 level */
	bzero(logtime,sizeof(logtime));
	think_gl_autoget(logbuf,loglen,6,logtime,sizeof(logtime),rulearray,rulecount,glflags);

	/* 7 level */
	loglevel=0;	/* normal */
	think_gl_autoget(logbuf,loglen,7,&loglevel,0,rulearray,rulecount,glflags);

	/* 8 desc */
	bzero(logdesc,sizeof(logdesc));
	think_gl_autoget(logbuf,loglen,8,logdesc,sizeof(logdesc),rulearray,rulecount,glflags);

	/* 9 info */
	bzero(loginfo,sizeof(loginfo));
	think_gl_autoget(logbuf,loglen,9,loginfo,sizeof(loginfo),rulearray,rulecount,glflags);

	think_db_new(db);
	think_db_putfield(db,0,p->centernet->net->ip,strlen(p->centernet->net->ip),0);
	think_db_putfield(db,1,&(p->pid),sizeof(p->pid),0);
	think_db_putfield(db,2,logtime,strlen(logtime),0);
	think_db_putfield(db,3,&loglevel,sizeof(loglevel),0);
	think_db_putfield(db,4,logdesc,strlen(logdesc),0);
	think_db_putfield(db,5,loginfo,strlen(loginfo),0);
	think_db_insert(db);

	/* send to logunit's viewers */
	cliplistp=p->logunit->viewlist;
	if(cliplistp==NULL)	/* no viewers */
		return 0;

	do{
		if(strlen(cliplistp->viewunit->ip)>0){
			if(strcmp(p->centernet->net->ip,cliplistp->viewunit->ip)!=0){
				cliplistp=cliplistp->next;
				continue;
			}
		}
		if(cliplistp->viewunit->pid>0){
			if(p->pid!=cliplistp->viewunit->pid){
				cliplistp=cliplistp->next;
				continue;
			}
		}
		if(loglevel<cliplistp->viewunit->level){
			cliplistp=cliplistp->next;
			continue;
		}
		if(strlen(cliplistp->viewunit->desc)>0){
			if(strcmp(logdesc,cliplistp->viewunit->desc)!=0){
				cliplistp=cliplistp->next;
				continue;
			}
		}
		if(strlen(cliplistp->viewunit->pattern)>0){
			if(strstr(loginfo,cliplistp->viewunit->pattern)==NULL){
				cliplistp=cliplistp->next;
				continue;
			}
		}
		loglen=0;
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,0,"50",0,rulearray,rulecount,THINK_GL_NOT_TRIM|glflags);
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,3,p->centernet->net->ip,0,rulearray,rulecount,glflags);
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,4,&p->pid,0,rulearray,rulecount,glflags);
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,6,logtime,0,rulearray,rulecount,glflags);
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,7,&loglevel,0,rulearray,rulecount,glflags);
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,8,logdesc,0,rulearray,rulecount,glflags);
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,9,loginfo,0,rulearray,rulecount,glflags);
		if(think_netcenter_sendmsg(cliplistp->cliunit->centernet,logbuf,loglen)<0)
			think_logsend(lp,THINK_LOG_ERROR,"network","%s",__think_errmsg__);
		cliplistp=cliplistp->next;
	}while(cliplistp!=p->logunit->viewlist);

	return 0;
}
int _logview(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen)
{
	char v_ip[16],v_time[20],v_desc[32],v_pattern[32];
	char logip[16],logtime[20],logdesc[32],loginfo[512];
	int v_pid,v_level;
	int logpid,loglevel;
	THINK_DB *db;
	char logbuf[1024];
	int loglen,r;
	struct cliunit_t *clip;
	struct viewlist_t **viewlist,*viewunit,*first,*last;
	struct logunit_t *logunit;
	char msgtype[3],v_name[128];
	int oflags;
	char subpath[256],fullpath[256],filename[256],file[256];
	char msgbufout[1024],respcode[3],resptext[128];
	int msglenout;

	think_logsend(lp,THINK_LOG_INFO,"system","logview ...");

	bzero(msgtype,sizeof(msgtype));
	think_gl_autoget(msgbuf,msglen,0,msgtype,sizeof(msgtype),rulearray,rulecount,glflags);

	bzero(v_name,sizeof(v_name));
	if(think_gl_autoget(msgbuf,msglen,1,v_name,sizeof(v_name),rulearray,rulecount,glflags)<0){
		think_logsend(lp,THINK_LOG_ERROR,"protocol","%s",__think_errmsg__);
		return -1;
	}
	think_logsend(lp,THINK_LOG_NORMAL,"business","logname=%s",v_name);
	if(strcmp(v_name,log_name)==0)
		think_logsend(lp,THINK_LOG_INFO,"business","viewing logserver's log!");

	bzero(v_ip,sizeof(v_ip));
	think_gl_autoget(msgbuf,msglen,3,v_ip,sizeof(v_ip),rulearray,rulecount,glflags);

	v_pid=0;
	think_gl_autoget(msgbuf,msglen,4,&v_pid,0,rulearray,rulecount,glflags);

	bzero(v_time,sizeof(v_time));
	think_gl_autoget(msgbuf,msglen,6,v_time,sizeof(v_time),rulearray,rulecount,glflags);

	v_level=THINK_LOG_NORMAL;	/* default level */
	think_gl_autoget(msgbuf,msglen,7,&v_level,0,rulearray,rulecount,glflags);

	bzero(v_desc,sizeof(v_desc));
	think_gl_autoget(msgbuf,msglen,8,v_desc,sizeof(v_desc),rulearray,rulecount,glflags);

	bzero(v_pattern,sizeof(v_pattern));
	think_gl_autoget(msgbuf,msglen,9,v_pattern,sizeof(v_pattern),rulearray,rulecount,glflags);

	clip=cliunitsearch(centernet);
	if(strcmp(msgtype,"30")==0){	/* query */
		think_logsend(lp,THINK_LOG_DEBUG,"business","this is a query.");
		if(strcmp(v_name,log_name)==0){
			db=lp->db;
		}else if((logunit=logunitsearch(v_name))==NULL){
			/* get subpath & filename */
			bzero(subpath,sizeof(subpath));
			bzero(filename,sizeof(filename));
			if(v_name[0]=='/'){
				think_logsend(lp,THINK_LOG_ERROR,"business","name illegal");
				return -1;
			}
			if((r=think_chrfind(v_name,'/',-1))<0){
				strcpy(subpath,"");
				strcpy(filename,v_name);
			}else{
				memcpy(subpath,v_name,r);
				strcpy(filename,v_name+r+1);
			}

			/* get fullpath */
			if(strlen(subpath)==0)
				strcpy(fullpath,log_path);
			else
				sprintf(fullpath,"%s/%s",log_path,subpath);

			/* get full file */
			sprintf(file,"%s/log%s.db",fullpath,filename);

			think_logsend(lp,THINK_LOG_NORMAL,"system","db opening ...");
			/* db */
			oflags=THINK_DB_RDONLY;
			if((db=think_db_open(file,oflags,NULL,0))==NULL){
				think_logsend(lp,THINK_LOG_ERROR,"system","%s",__think_errmsg__);
				return -1;
			}
		}else{
			db=logunit->db;
		}

		/* resp */
		strcpy(respcode,"00");
		strcpy(resptext,"succeed!");
		msglenout=0;
		msglenout+=think_gl_autoput(msgbufout,sizeof(msgbufout),0,"31",0,rulearray,rulecount,THINK_GL_NOT_TRIM|glflags);
		msglenout+=think_gl_autoput(msgbufout+msglenout,sizeof(msgbufout)-msglenout,126,respcode,0,rulearray,rulecount,glflags);
		msglenout+=think_gl_autoput(msgbufout+msglenout,sizeof(msgbufout)-msglenout,127,resptext,0,rulearray,rulecount,glflags);
		think_netcenter_sendmsg(centernet,msgbufout,msglenout);

		/* query */
		think_logsend(lp,THINK_LOG_NORMAL,"system","db querying ...");
		think_db_seek(db,0,THINK_DB_SEEK_SET);
		while(think_db_fetch(db)==1){
			think_db_getfield(db,0,logip,sizeof(logip),0);
			think_db_getfield(db,1,&logpid,sizeof(logpid),0);
			think_db_getfield(db,2,logtime,sizeof(logtime),0);
			think_db_getfield(db,3,&loglevel,sizeof(loglevel),0);
			think_db_getfield(db,4,logdesc,sizeof(logdesc),0);
			think_db_getfield(db,5,loginfo,sizeof(loginfo),0);
			if(strlen(v_time)>0){
				if(strcmp(logtime,v_time)<0)
					continue;
			}
			if(strlen(v_ip)>0){
				if(strcmp(logip,v_ip)!=0)
					continue;
			}
			if(strlen(v_time)>0){
				if(strcmp(logtime,v_time)<0)
					continue;
			}
			if(v_pid>0){
				if(logpid!=v_pid)
					continue;
			}
			if(loglevel<v_level)
				continue;
			if(strlen(v_desc)>0){
				if(strcmp(logdesc,v_desc)!=0)
					continue;
			}
			if(strlen(v_pattern)>0){
				if(strstr(loginfo,v_pattern)==NULL)
					continue;
			}
			loglen=0;
			loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,0,"31",0,rulearray,rulecount,THINK_GL_NOT_TRIM|glflags);
			loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,3,logip,0,rulearray,rulecount,glflags);
			loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,4,&logpid,0,rulearray,rulecount,glflags);
			loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,6,logtime,0,rulearray,rulecount,glflags);
			loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,7,&loglevel,0,rulearray,rulecount,glflags);
			loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,8,logdesc,0,rulearray,rulecount,glflags);
			loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,9,loginfo,0,rulearray,rulecount,glflags);
			if(think_netcenter_sendmsg(centernet,logbuf,loglen)<0){
				think_logsend(lp,THINK_LOG_ERROR,"network","%s",__think_errmsg__);
				if(logunit==NULL)
					think_db_close(db);
				return -1;
			}
		}
		if(strcmp(v_name,log_name)!=0 && logunit==NULL)
			think_db_close(db);
		think_logsend(lp,THINK_LOG_NORMAL,"business","query completed!");
		loglen=0;
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,0,"31",0,rulearray,rulecount,THINK_GL_NOT_TRIM|glflags); /* msgtype */
		loglen+=think_gl_autoput(logbuf+loglen,sizeof(logbuf)-loglen,10,"1",1,rulearray,rulecount,THINK_GL_FIXLEN|THINK_GL_NOT_TRIM|glflags); /* endflag */
		if(think_netcenter_sendmsg(centernet,logbuf,loglen)<0){
			think_logsend(lp,THINK_LOG_ERROR,"network","%s",__think_errmsg__);
			return -1;
		}
	}else{	/* viewer */
		think_logsend(lp,THINK_LOG_NORMAL,"business","this is a monitor.");
		if(strcmp(v_name,log_name)==0){
			think_logsend(lp,THINK_LOG_ERROR,"business","unsupported operation on logserver's log.");
			return -1;
		}
		clip->svflag='1';

		/* add to cliunit's viewlist */
		viewunit=malloc(sizeof(struct viewlist_t));
		bzero(viewunit,sizeof(struct viewlist_t));
		strcpy(viewunit->name,v_name);
		strcpy(viewunit->ip,v_ip);
		viewunit->pid=v_pid;
		viewunit->level=v_level;
		strcpy(viewunit->desc,v_desc);
		strcpy(viewunit->pattern,v_pattern);
		viewunit->prior=NULL;
		viewunit->next=NULL;
		viewlist=&clip->viewlist;
		if(*viewlist==NULL){
			*viewlist=viewunit;
			viewunit->prior=viewunit;
			viewunit->next=viewunit;
		}else{
			first=*viewlist;
			last=(*viewlist)->prior;
			viewunit->next=first;
			viewunit->prior=last;
			first->prior=viewunit;
			last->next=viewunit;
		}

		/* add to logunit's viewlist */
		think_logsend(lp,THINK_LOG_NORMAL,"business","adding to logname's viewlist.");
		logunit=loglist;
		if(loglist==NULL){
			think_logsend(lp,THINK_LOG_WARN,"business","waiting for log opened!");
		}else{
			do{
				if(strcmp(logunit->logname,v_name)==0){
					cliplistadd(&logunit->viewlist,clip,viewunit);
					viewunit->logunit=logunit;
					break;
				}
				logunit=logunit->next;
			}while(logunit!=loglist);
			if(viewunit->logunit)
				think_logsend(lp,THINK_LOG_NORMAL,"business","add ok.");
			else
				think_logsend(lp,THINK_LOG_WARN,"business","waiting for log opened!");
		}
		/* resp */
		strcpy(respcode,"00");
		strcpy(resptext,"succeed!");
		msglenout=0;
		msglenout+=think_gl_autoput(msgbufout,sizeof(msgbufout),0,"41",0,rulearray,rulecount,THINK_GL_NOT_TRIM|glflags);
		msglenout+=think_gl_autoput(msgbufout+msglenout,sizeof(msgbufout)-msglenout,126,respcode,0,rulearray,rulecount,glflags);
		msglenout+=think_gl_autoput(msgbufout+msglenout,sizeof(msgbufout)-msglenout,127,resptext,0,rulearray,rulecount,glflags);
		if(think_netcenter_sendmsg(centernet,msgbufout,msglenout)<0){
			think_logsend(lp,THINK_LOG_ERROR,"net","pkgsend error![%s]",__think_errmsg__);
			return -1;
		}
	}

	return 0;
}
int _logclose(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen)
{
	//close viewlists

	return 0;
}
int logunitadd(const char *logname,THINK_DB *db,int flags,const char *file)
{
	struct logunit_t *p,*first,*last;

	p=malloc(sizeof(struct logunit_t));
	bzero(p,sizeof(struct logunit_t));
	strcpy(p->logname,logname);
	p->db=db;
	strcpy(p->file,file);
	p->flags=flags;
	p->clilist=NULL;
	p->viewlist=NULL;
	p->prior=NULL;
	p->next=NULL;
	if(loglist==NULL){
		loglist=p;
		loglist->prior=loglist;
		loglist->next=loglist;
	}else{
		first=loglist;
		last=loglist->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}

	return 0;
}
int logunitdel(const char *logname)
{
	struct logunit_t *p,*prior,*next;

	p=logunitsearch(logname);
	if(p==p->next){
		free(p);
		loglist=NULL;
	}else{
		if(p==loglist)
			loglist=loglist->next;
		prior=p->prior;
		next=p->next;
		prior->next=next;
		next->prior=prior;
		free(p);
	}

	return 0;
}
struct logunit_t *logunitsearch(const char *logname)
{
	struct logunit_t *p;

	p=loglist;
	if(p==NULL)
		return NULL;
	do{
		if(strcmp(p->logname,logname)==0)
			return p;
		p=p->next;
	}while(p!=loglist);

	return NULL;
}
struct cliunit_t *cliunitsearch(THINK_NETCENTER_NET *centernet)
{
	struct cliunit_t *p;
	p=clilist;

	do{
		if(p->centernet==centernet)
			break;
		p=p->next;
	}while(p!=clilist);

	return p;
}
int cliunitdel(THINK_NETCENTER_NET *centernet)
{
	struct cliunit_t *p,*prior,*next;
	struct logunit_t *logunit;
	struct cliplist_t *_clilist,*viewplist,*nextclip,*priorclip;
	struct viewlist_t *viewlist,*nextviewlist,*priorviewlist;
	struct cliplist_t *priorviewplist,*nextviewplist;
	THINK_NET *net;

	net=centernet->net;
	think_netcenter_unregister(netcenter,centernet);
	think_netclose(net);
	p=cliunitsearch(centernet);
	if(p->svflag=='0'){ /* sender */
		think_logsend(lp,THINK_LOG_NORMAL,"system","removing a sender.");
		//remove logunit's cliuser
		think_logsend(lp,THINK_LOG_NORMAL,"system","remove from logunit's sender list.");
		logunit=p->logunit;
		_clilist=logunit->clilist;
		do{
			if(_clilist->cliunit!=p){
				_clilist=_clilist->next;
				continue;
			}
			if(_clilist==_clilist->next){ /* only me send to this logunit */
				free(_clilist);
				logunit->clilist=NULL;
			}else{
				if(_clilist==logunit->clilist)
					logunit->clilist=_clilist->next;
				priorclip=_clilist->prior;
				nextclip=_clilist->next;
				priorclip->next=nextclip;
				nextclip->prior=priorclip;
				free(_clilist);
			}
			break;
		}while(_clilist!=logunit->clilist);
		if(logunit->clilist==NULL){ /* close this logunit if no user send */
			think_logsend(lp,THINK_LOG_INFO,"business","close log because no sender.");
			/* set related viewflag to 0 */
			think_logsend(lp,THINK_LOG_NORMAL,"business","make viewer waiting.");
			viewplist=logunit->viewlist;
			while(viewplist){
				viewplist->viewunit->logunit=NULL;
				think_logsend(lp,THINK_LOG_WARN,"business","status changed to wait.%s:%d",viewplist->cliunit->centernet->net->ip,viewplist->cliunit->centernet->net->port);
				if(viewplist==viewplist->next){
					free(viewplist);
					break;
				}
				logunit->viewlist=viewplist->next;
				priorviewplist=viewplist->prior;
				nextviewplist=viewplist->next;
				priorviewplist->next=nextviewplist;
				nextviewplist->prior=priorviewplist;
				free(viewplist);
				viewplist=logunit->viewlist;
			}
			think_db_close(logunit->db);
			logunitdel(logunit->logname);
		}
	}else if(p->svflag=='1'){	/* viewer */
		think_logsend(lp,THINK_LOG_NORMAL,"business","removing a viewer.");
		think_logsend(lp,THINK_LOG_NORMAL,"business","removing from logunit's viewer list.");
		//remove all viewlists
		viewlist=p->viewlist;
		while(viewlist){
			if(viewlist->logunit){ /* viewing */
				viewplist=viewlist->logunit->viewlist;
				do{
					if(viewplist->viewunit!=viewlist){
						viewplist=viewplist->next;
						continue;
					}
					if(viewplist==viewplist->next){
						viewlist->logunit->viewlist=NULL;
						free(viewplist);
						break;
					}
					if(viewlist->logunit->viewlist==viewplist)
						viewlist->logunit->viewlist=viewplist->next;
					priorviewplist=viewplist->prior;
					nextviewplist=viewplist->next;
					priorviewplist->next=nextviewplist;
					nextviewplist->prior=priorviewplist;
					free(viewplist);
					break;
				}while(viewplist!=viewlist->logunit->viewlist);
			}
			if(viewlist==viewlist->next){ /* last unit */
				free(viewlist);
				break;
			}
			if(viewlist==p->viewlist)
				p->viewlist=viewlist->next;
			priorviewlist=viewlist->prior;
			nextviewlist=viewlist->next;
			priorviewlist->next=nextviewlist;
			nextviewlist->prior=priorviewlist;
			free(viewlist);
			viewlist=p->viewlist;
		}
	}
	if(p==p->next){
		free(p);
		clilist=NULL;
	}else{
		if(p==clilist)
			clilist=clilist->next;
		prior=p->prior;
		next=p->next;
		prior->next=next;
		next->prior=prior;
		free(p);
	}

	return 0;
}
int cliunitadd(THINK_NET *net)
{
	struct cliunit_t *p,*first,*last;

	p=malloc(sizeof(struct cliunit_t));
	bzero(p,sizeof(struct cliunit_t));
	p->svflag='\0';	/* no flag now */
	p->prior=NULL;
	p->next=NULL;
	if(clilist==NULL){
		clilist=p;
		clilist->prior=clilist;
		clilist->next=clilist;
	}else{
		first=clilist;
		last=clilist->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}

	p->centernet=think_netcenter_register(netcenter,net,clientnethandler,p);

	return 0;
}
int cliplistadd(struct cliplist_t **cliplist,struct cliunit_t *cliunit,struct viewlist_t *viewunit)
{
	struct cliplist_t *p,*first,*last;

	p=malloc(sizeof(struct cliplist_t));
	bzero(p,sizeof(struct cliplist_t));
	p->cliunit=cliunit;
	p->viewunit=viewunit;
	p->prior=NULL;
	p->next=NULL;
	if(*cliplist==NULL){
		*cliplist=p;
		p->prior=p;
		p->next=p;
	}else{
		first=*cliplist;
		last=(*cliplist)->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}
	return 0;
}
