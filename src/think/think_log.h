/*
 * think_log.h: Think Log Interface (use Think DB )
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 *	logtest.db
 *	ip,pid,time,level,desc,info	(this order is reasonable)
 *
 *	default format:(time,level,desc,info)
 *	[2008-03-31 09:30:00]-[0]-[program]-recordno=100
 *	[2008-03-31 11:30:00]-[1]-[net]-connected by 192.168.1.102
 *	[2008-03-31 11:30:01]-[2]-[user]-login ... [0001]
 *	[2008-03-31 11:30:02]-[1]-[user]-login succeeded!
 *	[2008-03-31 11:31:59]-[3]-[trans]-accountno=1121341200002112
 *	[2008-03-31 11:31:59]-[3]-[trans]-amount=10000000.00
 *	[2008-03-31 14:03:20]-[4]-[program]-file too big!
 *	[2008-03-31 14:03:20]-[5]-[program]-file open failed![errno=2]
 *	[2008-03-31 14:03:20]-[6]-[program]-shmget failed![errno=2]
 *
 *	examples:
 *	think_logopen("127.0.0.1:10000","ATM/trade",THINK_LOG_DEBUG);
 *	think_logopen("../log","trade",THINK_LOG_PRINT);
 *	think_logopen("../log","POS/trade",THINK_LOG_DBF|THINK_LOG_NORMAL);
 *	think_logsend(lp,THINK_LOG_NORMAL,"file","%s","/home/luojian/book.txt");
 *	think_logsend(lp,THINK_LOG_INFO,"connect","connected by %s:%d",ip,port);
 *	think_logsend(lp,THINK_LOG_KEY,"business","AccountNo:%s",AccountNo);
 *	think_logsend(lp,THINK_LOG_DEBUG,"","len=%d",len);
 *	think_logsend(lp,0,"","hello,world!");
 *	think_logclose(lp);
 */

#ifndef __THINK_LOG_H__
#define __THINK_LOG_H__

#include "think_os.h"
#include "think_db.h"
#include "think_net.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_LOG_DEBUG		1	/* debug */
#define THINK_LOG_NORMAL	2	/* normal(default) */
#define THINK_LOG_INFO		3	/* info */
#define THINK_LOG_KEY		4	/* key */
#define THINK_LOG_WARN		5	/* warn */
#define THINK_LOG_ERROR		6	/* error */
#define THINK_LOG_FATAL		7	/* fatal */

#define THINK_LOG_MASK_LEVEL	0x00000007	/* log level mask */

#define THINK_LOG_TRUNC		0x80000000	/* trunc */
#define THINK_LOG_EXCL		0x40000000	/* exclusive */
#define THINK_LOG_LOCAL		0x20000000	/* local */
#define THINK_LOG_PRINT		0x10000000	/* print to screen */
#define THINK_LOG_G			0x08000000	/* GL debug flag */

struct __think_log {
	THINK_NET *net;			/* net */
	THINK_DB *db;			/* local */
	char addr[128];			/* address */
	char name[32];			/* name */
	int flags;			/* open flags */
	int pid;			/* process id */
};
typedef struct __think_log THINK_LOG;

THINK_LOG *think_logopen(const char *addr,const char *name,int flags);
int think_logsend(THINK_LOG *lp,int flags,const char *desc,const char *fmt,...);
int think_logclose(THINK_LOG *lp);
int think_loggetflags(const char *strflags);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_LOG_H__ */
