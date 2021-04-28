#ifndef __LOGSERVER_H__
#define __LOGSERVER_H__

#include "think.h"

#ifdef __cplusplus
extern "C" {
#endif

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

struct cliplist_t{
	struct cliunit_t *cliunit;
	struct viewlist_t *viewunit;
	struct cliplist_t *prior;
	struct cliplist_t *next;
};
struct viewlist_t{
	char name[128];
	char ip[16];
	unsigned int pid;
	int level;
	char desc[32];
	char pattern[32];
	int viewflag;	/* viewing or not */
	struct logunit_t *logunit;	/* for quick search */
	struct cliunit_t *cliunit;	/* send info to client */
	struct viewlist_t *prior;
	struct viewlist_t *next;
};
struct cliunit_t{
	THINK_NETCENTER_NET *centernet;
	char svflag;	/* send or view */
	unsigned int pid;	/* sender's pid */
	struct logunit_t *logunit;	/* can only send to one log */
	struct viewlist_t *viewlist;	/* can view many logs */
	struct cliunit_t *prior;
	struct cliunit_t *next;
};
struct logunit_t{
	char logname[128];
	THINK_DB *db;
	int flags;
	char file[128];
	struct cliplist_t *viewlist;	/* many viewers (view) */
	struct cliplist_t *clilist;	/* many writes if not excl (send) */
	struct logunit_t *prior;
	struct logunit_t *next;
};

char cfgpath[256]="../etc",cfgfile[128]="logserver.cfg";
char log_name[256]="logserver",log_flags[128]="";
char log_path[256]="../log",lsn_addr[32]="*:10000";
struct cliunit_t *clilist=NULL;
struct logunit_t *loglist=NULL;
THINK_NETCENTER *netcenter=NULL;
THINK_LOG *lp=NULL;
int glflags=0;

THINK_DB_FIELD fieldarray[6]={
	{"IP",'C',15,0},
	{"PID",'I',0,0},
	{"Time",'C',19,0},
	{"Level",'I',0,0},
	{"Desc",'C',31,0},
	{"Info",'C',511,0}
};
int _logrecv(THINK_NETCENTER_NET *centernet,char *msgbuf,int msgsiz);
int _logopen(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen);
int _logsend(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen);
int _logview(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen);
int _logclose(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen);

int cliunitadd(THINK_NET *net);
struct cliunit_t *cliunitsearch(THINK_NETCENTER_NET *centernet);
int cliunitdel(THINK_NETCENTER_NET *centernet);

int logunitadd(const char *logname,THINK_DB *db,int flags,const char *file);
struct logunit_t *logunitsearch(const char *logname);
int logunitdel(const char *logname);

int logunit_cliunitadd(struct logunit_t *logunit,struct cliunit_t *cliunit);

int cliplistadd(struct cliplist_t **cliplist,struct cliunit_t *cliunit,struct viewlist_t *viewunit);

void acceptnewclient(THINK_NETCENTER_HANDLE handle);
void clientnethandler(THINK_NETCENTER_HANDLE handle);
int clientmsgprocess(THINK_NETCENTER_NET *centernet,char *msgbuf,int msglen);

#ifdef __cplusplus
}
#endif

#endif
