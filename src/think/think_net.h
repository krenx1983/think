/*
 * think_net.h: Think Net Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_NET_H__
#define __THINK_NET_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_NET_WAIT	0x80000000

struct __think_net {
	int sockfd;
	char ip[16];
	unsigned short port;
};
typedef struct __think_net THINK_NET;

int think_netstart(void);
int think_netstop(void);

THINK_NET *think_netconnect(const char *ip,unsigned short port);
THINK_NET *think_netlisten(const char *ip,unsigned short port);
THINK_NET *think_netaccept(THINK_NET *net);
int think_netrecv(THINK_NET *net,void *buf,unsigned int siz,int flags);
int think_netsend(THINK_NET *net,const void *buf,unsigned int len,int flags);
int think_netclose(THINK_NET *net);

int think_netsendmsg(THINK_NET *net,const void *buf,unsigned int len);
int think_netrecvmsg(THINK_NET *net,void *buf,unsigned int siz);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_NET_H__ */
