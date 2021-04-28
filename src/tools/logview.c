#include "think.h"

THINK_GL_RULE rulearray[]={
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
int rulecount=sizeof(rulearray)/sizeof(THINK_GL_RULE);

void logview(	const char *logip,
		int logpid,
		const char *logtime,
		int loglevel,
		const char *logdesc,
		const char *loginfo,
		char format);

int main(int argc,char *argv[])
{
	char usage[]="usage: logview [-GLm] [-a addr] [-I ip] [-P pid] [-t time] [-l level] [-d desc] [-p pattern] [-f format] [name]";
	char msgbuf[1024];
	int msglen,r,lineno;
	char path[256],subpath[256],fullpath[256],filename[256],fullname[256];
	char logip[16],logtime[20],logdesc[32],loginfo[1024];
	int logpid,loglevel;
	char respcode[3],resptext[128],endflag;
	THINK_NET *net;
	THINK_DB *db;
	char addr[256],ip[16],time[20],desc[32],pattern[128],format,name[256];
	int G_Exists,L_Exists,m_Exists;
	int I_Exists,P_Exists,t_Exists,l_Exists,d_Exists,p_Exists;
	int pid;
	int level;
	char servip[16];
	unsigned short servport;
	int glflags=0;
	THINK_OPTARRAY optarray[11]={
		{'G','C',NULL,0,0,&G_Exists},
		{'L','C',NULL,0,0,&L_Exists},
		{'m','C',NULL,0,0,&m_Exists},
		{'a','C',addr,sizeof(addr),THINK_OPT_ARG,NULL},
		{'I','C',ip,sizeof(ip),THINK_OPT_ARG,&I_Exists},
		{'P','I',&pid,0,THINK_OPT_ARG|THINK_OPT_NOT_NULL,&P_Exists},
		{'t','C',time,sizeof(time),THINK_OPT_ARG|THINK_OPT_NOT_NULL,&t_Exists},
		{'l','I',&level,0,THINK_OPT_ARG|THINK_OPT_NOT_NULL,&l_Exists},
		{'d','C',desc,sizeof(desc),THINK_OPT_ARG,&d_Exists},
		{'p','C',pattern,sizeof(pattern),THINK_OPT_ARG|THINK_OPT_NOT_NULL,&p_Exists},
		{'f','C',&format,1,THINK_OPT_ARG|THINK_OPT_NOT_NULL|THINK_OPT_NOT_STRING,NULL}
	};
	THINK_ENVARRAY envarray[2]={
		{"LOG_ADDR",'C',addr,sizeof(addr),0,NULL},
		{"LOG_NAME",'C',name,sizeof(name),0,NULL}
	};
	
	bzero(addr,sizeof(addr));
	bzero(ip,sizeof(ip));
	pid=0;
	bzero(time,sizeof(time));
	level=0;
	bzero(desc,sizeof(desc));
	bzero(pattern,sizeof(pattern));
	format=0;
	bzero(name,sizeof(name));

	if(think_getenvs(envarray,2)<0){
		think_showerror();
		exit(-1);
	}
	if(think_getopts(argc,argv,optarray,11)<0){
		think_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind>1){
		puts(usage);
		exit(-1);
	}
	if(argc-optind==1)
		strcpy(name,argv[optind]);

	/* GL flags */
	if(G_Exists)
		glflags|=THINK_GL_DEBUG;

	if(L_Exists){	/* local */
		/* get path */
		strcpy(path,addr);
		if(strlen(path)>1 && path[strlen(path)-1]=='/')
			path[strlen(path)-1]='\0';	/* delete last separate char '/' */
		if(strlen(path)==0)
			strcpy(path,".");	/* current dir */

		/* get subpath & filename */
		bzero(subpath,sizeof(subpath));
		bzero(filename,sizeof(filename));
		if(name[0]=='/'){
			printf("name illegal.[%s]\n",name);
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
			strcpy(fullpath,path);
		else
			sprintf(fullpath,"%s/%s",path,subpath);
		if(think_automkdir(fullpath,0777)<0){	/* auto mkdir */
			think_showerror();
			return -1;
		}

		/* get full file */
		sprintf(fullname,"%s/log%s.db",fullpath,filename);
		if((db=think_db_openx(fullname,THINK_DB_RDONLY,NULL))==NULL){
			think_showerror();
			return -1;
		}
		lineno=1;
		while(think_db_fetch(db)==1){
			think_db_getfield(db,0,logip,sizeof(logip),0);
			think_db_getfield(db,1,&logpid,sizeof(logpid),0);
			think_db_getfield(db,2,logtime,sizeof(logtime),0);
			think_db_getfield(db,3,&loglevel,sizeof(loglevel),0);
			think_db_getfield(db,4,logdesc,sizeof(logdesc),0);
			think_db_getfield(db,5,loginfo,sizeof(loginfo),0);
			lineno++;

			if(I_Exists && strcmp(logip,ip)!=0)
				continue;
			if(P_Exists && logpid!=pid)
				continue;
			if(t_Exists && strcmp(logtime,time)<0)
				continue;
			if(l_Exists && loglevel<level)
				continue;
			if(d_Exists && strcmp(logdesc,desc)!=0)
				continue;
			if(p_Exists && strstr(loginfo,pattern)==NULL)
				continue;
			logview(logip,logpid,logtime,loglevel,logdesc,loginfo,format);
		}
		think_db_close(db);
	}else{	/* NET */
		/* init */
		msglen=0;

		/* 0 msgtype */
		if(m_Exists)	/* monitor */
			msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,0,"40",0,rulearray,rulecount,THINK_GL_NOT_TRIM|glflags);
		else	/* query */
			msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,0,"30",0,rulearray,rulecount,THINK_GL_NOT_TRIM|glflags);

		/* 1 name */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,1,name,0,rulearray,rulecount,glflags);

		/* 3 ip */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,3,ip,0,rulearray,rulecount,glflags);

		/* 4 pid */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,4,&pid,0,rulearray,rulecount,glflags);

		/* 6 time */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,6,time,0,rulearray,rulecount,glflags);

		/* 7 level */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,7,&level,0,rulearray,rulecount,glflags);

		/* 8 desc */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,8,desc,0,rulearray,rulecount,glflags);

		/* 9 pattern */
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,9,pattern,0,rulearray,rulecount,glflags);

		/* ip:port */
		strcpy(servip,"127.0.0.1");
		servport=10000;
		if(strlen(addr)==0)
			strcpy(addr,"127.0.0.1:10000");
		think_strgetfield(addr,':',0,'C',servip,sizeof(servip),0);
		think_strgetfield(addr,':',1,'S',&servport,0,0);

		/* net start */
		think_netstart();

		/* connect */
		if((net=think_netconnect(servip,servport))==NULL){
			think_showerror();
			return -1;
		}

		/* send package */
		if(think_netsendmsg(net,msgbuf,msglen)<0){
			think_showerror();
			think_netclose(net);
			return -1;
		}
		if((msglen=think_netrecvmsg(net,msgbuf,sizeof(msgbuf)))<0){
			think_showerror();
			think_netclose(net);
			return -1;
		}
		bzero(respcode,sizeof(respcode));
		think_gl_autoget(msgbuf,msglen,126,respcode,sizeof(respcode),rulearray,rulecount,glflags);
		bzero(resptext,sizeof(resptext));
		think_gl_autoget(msgbuf,msglen,127,resptext,sizeof(resptext),rulearray,rulecount,glflags);
		if(strcmp(respcode,"00")!=0){
			think_error(0,"[%s]:[%s:%s]",__func__,respcode,resptext);
			think_netclose(net);
			return -1;
		}
		endflag='0';
		while(1){
			if((msglen=think_netrecvmsg(net,msgbuf,sizeof(msgbuf)))<0){
				think_showerror();
				break;
			}
			bzero(logip,sizeof(logip));
			logpid=0;
			bzero(logtime,sizeof(logtime));
			loglevel=0;
			bzero(logdesc,sizeof(logdesc));
			bzero(loginfo,sizeof(loginfo));

			think_gl_autoget(msgbuf,msglen,3,logip,sizeof(logip),rulearray,rulecount,glflags);
			think_gl_autoget(msgbuf,msglen,4,&logpid,0,rulearray,rulecount,glflags);
			think_gl_autoget(msgbuf,msglen,6,logtime,sizeof(logtime),rulearray,rulecount,glflags);
			think_gl_autoget(msgbuf,msglen,7,&loglevel,0,rulearray,rulecount,glflags);
			think_gl_autoget(msgbuf,msglen,8,logdesc,sizeof(logdesc),rulearray,rulecount,glflags);
			think_gl_autoget(msgbuf,msglen,9,loginfo,sizeof(loginfo),rulearray,rulecount,glflags);
			think_gl_autoget(msgbuf,msglen,10,&endflag,1,rulearray,rulecount,THINK_GL_NOT_STRING|glflags);
			if(endflag=='1')
				break;
			logview(logip,logpid,logtime,loglevel,logdesc,loginfo,format);
		}
		think_netclose(net);

		/* net stop */
		think_netstop();
	}

	return 0;
}

void logview(	const char *logip,
		int logpid,
		const char *logtime,
		int loglevel,
		const char *logdesc,
		const char *loginfo,
		char format)
{
	char prompt[128];
	int pos,r;

	/* view mode */
	switch(format){
		case '0':
			sprintf(prompt,"[%s]-[%d]-[%s]-",logtime,loglevel,logdesc);
			break;
		case '1':
			sprintf(prompt,"[%s]-",logtime);
			break;
		case '2':
			sprintf(prompt,"%s","");
			break;
		case '3':
			sprintf(prompt,"[%-15s]-[%-5d]-[%s]-[%d]-[%s]-",logip,logpid,logtime,loglevel,logdesc);
			break;
		default:
			sprintf(prompt,"[%s]-[%d]-[%s]-",logtime,loglevel,logdesc);
			break;
	};

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
