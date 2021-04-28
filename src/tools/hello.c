#include "think.h"

#define HELLO_HARDWARE	"X86"
#define HELLO_OS		"Windows"
#define HELLO_APP		"Hello"
#define HELLO_VERSION	"1.0"

char hello_protocol_no=0;
char hello_protocol_version=0;
THINK_UINT currseqno=0;

THINK_GL_RULE rulearray[]={
	{0	,"transcode",'C'	,1		,THINK_GL_BINARY	,"trans code"},
	{1	,"user"		,'C'	,30		,0					,"user name"},
	{2	,"message"	,'C'	,8000	,THINK_GL_BINARY	,"message"},
	{3	,"status"	,'C'	,1		,THINK_GL_BINARY	,"status code"},
	{4	,"group"	,'C'	,30		,0					,"group name"},
	{5	,"status"	,'C'	,1		,THINK_GL_BINARY	,"group status code"},
	{6	,"title"	,'C'	,30		,0					,"broadcast title"},
	{7	,"text"		,'C'	,100	,0					,"broadcast text"},
	{8	,"msgtype"	,'C'	,1		,THINK_GL_BINARY	,"message type"},
	{9	,"sessionid",'I'	,0		,THINK_GL_UNSIGNED	,"session id"},
	{10	,"seqno"	,'I'	,0		,THINK_GL_UNSIGNED	,"sequence no"},
	{11	,"passwd"	,'C'	,15		,0					,"passwd"},
	{12	,"date"		,'C'	,10		,0					,"date"},
	{13	,"time"		,'C'	,12		,0					,"time"},
	{14	,"hardware"	,'C'	,15		,0					,"hardware"},
	{15	,"onlinecount",'I'	,0		,THINK_GL_UNSIGNED	,"online count"},
	{16	,"os"		,'C'	,15		,0					,"os"},
	{17	,"appname"	,'C'	,15		,0					,"app name"},
	{18	,"appversion",'C'	,15		,0					,"app version"},
	{19	,"protocol"	,'C'	,1		,THINK_GL_BINARY	,"protocol"},
	{20	,"transtype",'C'	,1		,0					,"trans type"},
	{21	,"file"		,'C'	,1024	,0					,"file"},
	{22	,"dirflag"	,'C'	,1		,0					,"dir flag"},
	{23	,"msgno"	,'I'	,0		,0					,"message no"},
	{24	,"filesize"	,'I'	,0		,0					,"file size"},
	{25	,"filetext"	,'C'	,7000	,THINK_GL_BINARY	,"file text"},
	{26	,"ioflag"	,'C'	,1		,0					,"io flag"},
	{27	,"version"	,'C'	,1		,0					,"protocol version"},
	{28	,"readytype",'C'	,1		,0					,"ready type"},
	{29	,"orderseq"	,'I'	,0		,0					,"order sequence"},
	{30	,"bsflag"	,'C'	,1		,0					,"bsflag"},
	{31	,"ocflag"	,'C'	,1		,0					,"ocflag"},
	{32	,"pricetype",'C'	,1		,0					,"pricetype"},
	{33	,"price"	,'D'	,0		,0					,"price"},
	{34	,"volume"	,'I'	,0		,0					,"volume"},
	{35	,"orderno"	,'I'	,0		,0					,"orderno"},
	{36	,"volumefilled",'I'	,0		,0					,"volumefilled"},
	{37	,"orderstatus",'C'	,1		,0					,"orderstatus"},
	{38	,"matchno"	,'I'	,0		,0					,"matchno"},
	{39	,"totvolume",'I'	,0		,0					,"totvolume"},
	{40	,"open"		,'D'	,0		,0					,"open"},
	{41	,"high"		,'D'	,0		,0					,"high"},
	{42	,"low"		,'D'	,0		,0					,"low"},
	{43	,"close"	,'D'	,0		,0					,"close"},
	{44	,"lastclose",'D'	,0		,0					,"lastclose"},
	{45	,"lastsettle",'D'	,0		,0					,"lastsettle"},
	{46	,"settle"	,'D'	,0		,0					,"settle"},
	{47	,"oi"		,'I'	,0		,0					,"oi"},
	{48	,"quoteseq"	,'I'	,0		,0					,"quoteseq"},
	{49	,"tradestatus",'C'	,1		,0					,"tradestatus"},
	{50	,"buypos"	,'I'	,0		,0					,"buypos"},
	{51	,"buyprice"	,'D'	,0		,0					,"buyprice"},
	{52	,"sellpos"	,'I'	,0		,0					,"sellpos"},
	{53	,"sellprice",'D'	,0		,0					,"sellprice"},
	{54	,"closepl"	,'D'	,0		,0					,"closepl"},
	{55	,"floatingpl",'D'	,0		,0					,"floatingpl"},
	{56	,"todaypl"	,'D'	,0		,0					,"todaypl"},
	{57	,"totpl"	,'D'	,0		,0					,"totpl"},
	{58	,"lastequity",'D'	,0		,0					,"lastequity"},
	{59	,"equity"	,'D'	,0		,0					,"equity"},
	{60	,"equityfree",'D'	,0		,0					,"equityfree"},
	{61	,"occupymargin",'D'	,0		,0					,"occupymargin"},
	{62	,"frozenmargin",'D'	,0		,0					,"frozenmargin"},
	{63	,"occupyfee",'D'	,0		,0					,"occupyfee"},
	{64	,"frozenfee",'D'	,0		,0					,"frozenfee"},
	{65	,"fee"		,'D'	,0		,0					,"fee"},
	{66	,"totfee"	,'D'	,0		,0					,"totfee"},
	{67	,"closepl"	,'D'	,0		,0					,"closepl"},
	{68	,"floatingpl",'D'	,0		,0					,"floatingpl"},
	{69	,"todaypl"	,'D'	,0		,0					,"todaypl"},
	{70	,"totpl"	,'D'	,0		,0					,"totpl"},
	{71	,"top10type",'C'	,1		,0					,"top10type"},
	{72	,"top10value",'I'	,0		,0					,"top10value"},
	{73	,"bulletinseq",'I'	,0		,0					,"bulletinseq"},
	{74	,"bullettitle",'C'	,100	,0					,"bullettitle"},
	{75	,"accseq"	,'I'	,0		,0					,"accseq"},
	{76	,"datefrom"	,'C'	,10		,0					,"datefrom"},
	{77	,"dateto"	,'C'	,12		,0					,"dateto"},
	{78	,"ioflag"	,'C'	,1		,0					,"ioflag"},
	{79	,"amount"	,'D'	,0		,0					,"amount"},
	{80	,"desc"		,'C'	,100	,0					,"description"},
	{81	,"endflag"	,'C'	,1		,0					,"endflag"},
	{82	,"bullettext",'C'	,4000	,0					,"bullettext"},
	{83	,"pos"		,'I'	,0		,0					,"file pos"},
	{84	,"offline"	,'C'	,1		,THINK_GL_BINARY	,"offline flag"},
	{85	,"writer"	,'C'	,39		,0					,"message writer"},
	{86	,"newpasswd",'C'	,15		,0					,"new password"},
	{87	,"syncresult",'C'	,1		,0					,"sync result"},
	{88	,"syncdesc", 'C'	,100	,0					,"sync result desc"},
	{89	,"ordertime",'C'	,12		,0					,"order time"},
	{90	,"matchtime",'C'	,12		,0					,"match time"},
	{91	,"qtyleft"	,'I'	,0		,0					,"qty left"},
	{92	,"avgprice"	,'D'	,0		,0					,"avg price"},
	{93	,"initoi"	,'I'	,0		,0					,"init oi"},
	{94	,"noteno"	,'I'	,0		,0					,"note no"},
	{95	,"notetext"	,'C'	,8000	,0					,"note text"},
	{96	,"weblogno"	,'I'	,0		,0					,"weblog no"},
	{97	,"weblogtext",'C'	,8000	,0					,"weblog text"},
	{98	,"comment"	,'C'	,8000	,0					,"weblog comment"},
	{99	,"respcode"	,'S'	,0		,0					,"response code"},
	{100,"resptext"	,'C'	,100	,0					,"response text"},
	{101,"board"	,'C'	,30		,0					,"board name"},
	{102,"postno"	,'I'	,0		,0					,"post no"},
	{103,"posttext"	,'C'	,8000	,0					,"post text"},
	{104,"comment"	,'C'	,8000	,0					,"post comment"},
	{105,"choice"	,'C'	,1		,0					,"love choice"},
	{106,"yescount"	,'I'	,0		,0					,"yes count"},
	{107,"nocount"	,'I'	,0		,0					,"no count"},
	{108,"alivetime",'S'	,0		,THINK_GL_UNSIGNED	,"alive time"},
	{109,"vhistory"	,'C'	,4000	,0					,"version history"},
	{110,"symbol"	,'C'	,30		,0					,"symbol"},
	{111,"variety"	,'C'	,30		,0					,"variety"},
	{112,"tradeunit",'D'	,0		,0					,"trade unit"},
	{113,"priceunit",'D'	,0		,0					,"price unit"},
	{114,"pricecoeff",'D'	,0		,0					,"price coefficient"},
	{115,"pricescale",'D'	,0		,0					,"price scale"},
	{116,"minmove"	,'D'	,0		,0					,"min movement"},
	{117,"dailylimit",'D'	,0		,0					,"daily limit"},
	{118,"marginrate",'D'	,0		,0					,"margin rate"},
	{119,"feetype"	,'D'	,0		,0					,"fee type"},
	{120,"fee"		,'D'	,0		,0					,"fee"},
	{121,"marketstatus"	,'C',1		,0					,"market status"},
	{122,"deepquote",'C'	,1000	,THINK_GL_BINARY	,"deep quote"},
	{123,"highlimit",'D'	,0		,0					,"high limit"},
	{124,"lowlimit"	,'D'	,0		,0					,"low limit"}
};
int rulecount=sizeof(rulearray)/sizeof(THINK_GL_RULE);	
	
char serv_addr[128]="116.247.97.242:1314";
int glflags=0,isdebuging=0,isquiet=0;
THINK_NETCENTER *netcenter;
THINK_NET *net;
THINK_NETCENTER_NET *centernet;
char me[32]="anonymous",password[32]="",buddy[32]="",lastbuddy[32]="";
int islogined=0,isbinded=0;

void clientnethandler(THINK_NETCENTER_HANDLE handle);
int clientlogin(const char *user,const char *passwd);
int handleloginrsp(const char *msgbuf,int msglen);
int handlemessage(const char *msgbuf,int msglen);
int handlestatusnty(const char *msgbuf,int msglen);
int handlemsgnty(const char *msgbuf,int msglen);
int clientmsgtransfer(const char *buddy,const char *msg);
int handlecmdline(const char *cmdline);
void handleinput(void *p);
void display_all_commands(void);
int handlemsgtransferrsp(const char *msgbuf,int msglen);
int clientmsgsubscribereq(unsigned char msgtype,THINK_INT msgno);

int main(int argc,char *argv[])
{
	char usage[]="usage: hello [-dGq] [-a addr] [-u user] [-p passwd] [buddy]";
	THINK_USHORT servport;
	char servip[128];
	int G_Exists,d_Exists,q_Exists;
	THINK_ENVARRAY envarray[2]={
		{"HELLO_USER",'C',me,sizeof(me),THINK_ENV_NOT_NULL,NULL},
		{"HELLO_ADDR",'C',serv_addr,sizeof(serv_addr),THINK_ENV_NOT_NULL,NULL}
	};
	THINK_OPTARRAY optarray[6]={
		{'G','C',NULL,0,0,&G_Exists},
		{'d','C',NULL,0,0,&d_Exists},
		{'q','C',NULL,0,0,&q_Exists},
		{'u','C',me,sizeof(me),THINK_OPT_ARG|THINK_OPT_NOT_NULL,NULL},
		{'p','C',password,sizeof(password),THINK_OPT_ARG|THINK_OPT_NOT_NULL,NULL},
		{'a','C',serv_addr,sizeof(serv_addr),THINK_OPT_ARG|THINK_OPT_NOT_NULL,NULL}
	};

#ifdef __THINK_UNIX__
	signal(SIGPIPE,SIG_IGN);
#endif
	
	if(think_getenvs(envarray,2)<0){
		think_showerror();
		return -1;
	}
	if(think_getopts(argc,argv,optarray,6)<0){
		think_showerror();
		puts(usage);
		return -1;
	}
	if(argc-optind>1){
		puts(usage);
		exit(-1);
	}
	if(argc-optind==1){
		strcpy(buddy,argv[optind]);
		isbinded=1;
	}
	if(strlen(me)==0)
		strcpy(me,"anonymous");
	if(G_Exists)
		glflags|=THINK_GL_DEBUG;
	if(d_Exists)
		isdebuging=1;
	if(q_Exists)
		isquiet=1;
	
	/* net start */
	think_netstart();
	
	/*listening...*/
	think_strgetfield(serv_addr,':',0,'C',servip,sizeof(servip),0);
	think_strgetfield(serv_addr,':',1,'S',&servport,0,0);
	if((net=think_netconnect(servip,servport))==NULL){
		think_showerror();
		return -1;
	}
	netcenter=think_netcenter_new();
	centernet=think_netcenter_register(netcenter,net,clientnethandler,net);
	clientlogin(me,password);
	think_threadcreate(handleinput,NULL);
	while(1){
		if(think_netcenter_idle(netcenter,10)<0){
			think_showerror();
			break;
		}
	}
	
	/* net stop */
	think_netstop();
	
	return 0;
}
void handleinput(void *p)
{
	char cmdline[8192];

	while(1){
		bzero(cmdline,sizeof(cmdline));
		if(fgets(cmdline,sizeof(cmdline),stdin)==NULL)
			break;
		think_strtrim(cmdline);
		if(handlecmdline(cmdline)<0)
			think_showerror();
	}
}
void clientnethandler(THINK_NETCENTER_HANDLE handle)
{
	char msgbuf[8192];
	int msglen;
	int r;
	
	think_netcenter_recvto(centernet);
	/* check msg */
	while((r=think_netcenter_ismsgok(centernet))!=0){
		if(r<0){
			think_showerror();
			exit(-1);
		}
		msglen=think_netcenter_recvmsg(centernet,msgbuf,sizeof(msgbuf));
		/* process msg */
		if(handlemessage(msgbuf,msglen)<0){
			think_showerror();
			exit(-1);
		}
	}
}
int handlecmdline(const char *cmdline)
{
	char cmd[8192];

	if(strcmp(cmdline,"exit")==0 || strcmp(cmdline,"quit")==0)
		exit(0);

	if(strcmp(cmdline,"unbind")==0){
		isbinded=0;
		return 0;
	}
	if(isbinded){
		if(!islogined){
			printf("not logined yet!\n");
			return 0;
		}
		clientmsgtransfer(buddy,cmdline);
		return 0;
	}
	if(think_strgetfield(cmdline,' ',0,'C',cmd,sizeof(cmd),0)<0)
		return -1;
	if(strcmp(cmd,"bind")==0){
		if(think_strgetfield(cmdline,' ',1,'C',buddy,sizeof(buddy),0)<0)
			return -1;
		isbinded=1;
	}else if(strcmp(cmd,"to")==0){
		if(!islogined){
			printf("not logined yet!\n");
			return 0;
		}
		if(think_chrstat(cmdline,' ')<1){
			printf("Usage: to buddy message\n");
			return 0;
		}
		if(think_strgetfield(cmdline,' ',1,'C',buddy,sizeof(buddy),0)<0)
			return -1;
		if(think_chrstat(cmdline,' ')>=2)
			clientmsgtransfer(buddy,cmdline+(think_chrfind(cmdline,' ',1)+1));
		else
			clientmsgtransfer(buddy,"");
	}else if(strcmp(cmd,"help")==0){
		display_all_commands();
	}else{
		if(strlen(lastbuddy)==0)
			display_all_commands();
		else
			clientmsgtransfer(lastbuddy,cmdline);
	}

	return 0;
}
void display_all_commands(void)
{
	printf("\
******************************************\n\
Hello commands:\n\
******************************************\n\
1. [message]\n\
2. to buddy [message]\n\
3. bind buddy\n\
4. unbind\n\
5. help\n\
5. quit/exit\n\
******************************************\n\
");
}
int clientlogin(const char *user,const char *passwd)
{
	char type,msgbuf[8192];
	int msglen;
	
	msglen=0;
	type=1; /* Login */
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,0,&type,1,rulearray,rulecount,glflags|THINK_GL_NOT_STRING);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,1,user,0,rulearray,rulecount,glflags|THINK_GL_NOT_TRIM);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,11,passwd,0,rulearray,rulecount,glflags|THINK_GL_NOT_TRIM);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,14,HELLO_HARDWARE,0,rulearray,rulecount,glflags|THINK_GL_NOT_TRIM);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,16,HELLO_OS,0,rulearray,rulecount,glflags|THINK_GL_NOT_TRIM);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,17,HELLO_APP,0,rulearray,rulecount,glflags|THINK_GL_NOT_TRIM);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,18,HELLO_VERSION,0,rulearray,rulecount,glflags|THINK_GL_NOT_TRIM);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,19,&hello_protocol_no,1,rulearray,rulecount,glflags|THINK_GL_NOT_STRING);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,27,&hello_protocol_version,1,rulearray,rulecount,glflags|THINK_GL_NOT_STRING);
	
	think_netcenter_sendmsg(centernet,msgbuf,msglen);
	
	return 0;
}
int clientmsgtransfer(const char *buddy,const char *msg)
{
	char type,msgbuf[8192],msgtype,online=0;
	struct tm *t;
	time_t tt;
	char strdate[11],strtime[13];
	int msglen;
	
	msglen=0;
	type=3; /* Message Transfer */
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,0,&type,1,rulearray,rulecount,glflags);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,1,buddy,0,rulearray,rulecount,glflags);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,2,msg,strlen(msg),rulearray,rulecount,glflags);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,84,&online,1,rulearray,rulecount,glflags);
	
	tt=time(NULL);
	t=localtime(&tt);
	sprintf(strdate,"%04d-%02d-%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
	sprintf(strtime,"%02d:%02d:%02d",t->tm_hour,t->tm_min,t->tm_sec);

	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,12,strdate,0,rulearray,rulecount,glflags);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,13,strtime,0,rulearray,rulecount,glflags);

	msgtype=0;
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,8,&msgtype,1,rulearray,rulecount,glflags);
	
	think_netcenter_sendmsg(centernet,msgbuf,msglen);
	
	return 0;
}
int handlemessage(const char *msgbuf,int msglen)
{
	char msgtype;
	
	bzero(&msgtype,sizeof(msgtype));
	if(think_gl_autoget(msgbuf,msglen,0,&msgtype,1,rulearray,rulecount,glflags)<0){
		return -1;
	}
	switch(msgtype){
	case 0:	/* Heart Beat */
		break;
	case 1:	/* Login */
		if(handleloginrsp(msgbuf,msglen)<0)
			return -1;
		break;
	case 2: /* Status Notice */
		if(handlestatusnty(msgbuf,msglen)<0)
			return -1;
		break;
	case 3: /* Message Transfer Response*/
		if(handlemsgtransferrsp(msgbuf,msglen)<0)
			return -1;
		break;
	case 4: /* Message Transfer Notice */
		if(handlemsgnty(msgbuf,msglen)<0)
			return -1;
		break;
	default:
		break;
	}

	return 0;
}

int handleloginrsp(const char *msgbuf,int msglen)
{
	char resptext[128];
	THINK_USHORT respcode;
	
	if(think_gl_autoget(msgbuf,msglen,99,&respcode,0,rulearray,rulecount,glflags)<0){
		return -1;
	}
	
	bzero(resptext,sizeof(resptext));
	if(think_gl_autoget(msgbuf,msglen,100,resptext,sizeof(resptext),rulearray,rulecount,glflags)<0){
		return -1;
	}
	
	islogined=1;
	if(!isquiet)
		printf("login response:%hd-%s\n",respcode,resptext);
	if(respcode==0)
		clientmsgsubscribereq(0,-1);
	return 0;
}

int handlestatusnty(const char *msgbuf,int msglen)
{
	char buddy[32],status;
	THINK_UINT onlinecount;
	
	bzero(buddy,sizeof(buddy));
	if(think_gl_autoget(msgbuf,msglen,1,buddy,sizeof(buddy),rulearray,rulecount,glflags)<0){
		return -1;
	}
	
	if(think_gl_autoget(msgbuf,msglen,3,&status,1,rulearray,rulecount,glflags)<0){
		return -1;
	}
	
	think_gl_autoget(msgbuf,msglen,15,&onlinecount,0,rulearray,rulecount,glflags);
	if(status==0 && onlinecount>0)
		return 0;
	if(status==0 || status==3)
		printf("status notice:%s%s\n",buddy,"离线了！");
	else
		printf("status notice:%s%s\n",buddy,"上线了！");
	fflush(stdout);

	return 0;
}


int handlemsgnty(const char *msgbuf,int msglen)
{
	char buddy[32],sender[32],msg[8001],msgtype;
	THINK_UINT sessionid;
	
	bzero(buddy,sizeof(buddy));
	if(think_gl_autoget(msgbuf,msglen,1,buddy,sizeof(buddy),rulearray,rulecount,glflags)<0){
		return -1;
	}
	
	if(think_gl_autoget(msgbuf,msglen,9,&sessionid,0,rulearray,rulecount,glflags)<0){
		bzero(sender,sizeof(sender));
		if(think_gl_autoget(msgbuf,msglen,85,sender,sizeof(sender),rulearray,rulecount,glflags)<0){
			return 0;
		}
	}else{
		strcpy(sender,buddy);
	}
	
	bzero(msg,sizeof(msg));
	think_gl_autoget(msgbuf,msglen,2,msg,sizeof(msg),rulearray,rulecount,glflags);

	bzero(&msgtype,sizeof(msgtype));
	if(think_gl_autoget(msgbuf,msglen,8,&msgtype,1,rulearray,rulecount,glflags)<0){
		return 0;
	}
	if(msgtype!=0)
		return 0;
	
	strcpy(lastbuddy,buddy);
	printf("From %s: %s\n",sender,msg);
	
	return 0;
}
int handlemsgtransferrsp(const char *msgbuf,int msglen)
{
	char resptext[128];
	THINK_USHORT respcode;
	
	if(think_gl_autoget(msgbuf,msglen,99,&respcode,0,rulearray,rulecount,glflags)<0){
		return -1;
	}
	
	bzero(resptext,sizeof(resptext));
	if(think_gl_autoget(msgbuf,msglen,100,resptext,sizeof(resptext),rulearray,rulecount,glflags)<0){
		return -1;
	}
	
	if(isdebuging)
		printf("message transfer response:%hd-%s\n",respcode,resptext);

	return 0;
}

int clientmsgsubscribereq(unsigned char msgtype,THINK_INT msgno)
{
	char type,msgbuf[8192];
	int msglen;
	THINK_UINT seqno=currseqno++;
	
	if(!islogined)
		return -1;
	msglen=0;
	type=38; /* Subscribe Messages */
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,0,&type,1,rulearray,rulecount,glflags);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,10,&seqno,0,rulearray,rulecount,glflags);
	msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,8,&msgtype,1,rulearray,rulecount,glflags);
	if(msgno>=0)
		msglen+=think_gl_autoput(msgbuf+msglen,sizeof(msgbuf)-msglen,23,&msgno,0,rulearray,rulecount,glflags);
	
	think_netcenter_sendmsg(centernet,msgbuf,msglen);
	
	return 0;
}
