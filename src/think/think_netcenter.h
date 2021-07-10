/*
 * think_netcenter.h: Think Net Center Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_NETCENTER_H__
#define __THINK_NETCENTER_H__

#include "think_os.h"
#include "think_net.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_NETCENTER_BUFSIZE	8192

/* Max Connections */
#ifndef THINK_NETCENTER_MAX_CONNECTIONS 
#define THINK_NETCENTER_MAX_CONNECTIONS 1024
#endif

struct __think_netcenter_buflist {
	char *buf;
	unsigned int pos;
	unsigned int len;
	struct __think_netcenter_buflist *prior;
	struct __think_netcenter_buflist *next;
};
typedef struct __think_netcenter_buflist THINK_NETCENTER_BUFLIST;

typedef void *THINK_NETCENTER_HANDLE;
typedef void (*THINK_NETCENTER_HANDLER)(THINK_NETCENTER_HANDLE);

struct __think_netcenter;
struct __think_netcenter_netlist;
struct __think_netcenter_net {
	THINK_NET *net;
	THINK_NETCENTER_BUFLIST *rlist;
	THINK_NETCENTER_BUFLIST *wlist;
	unsigned int nrbytes;
	unsigned int nwbytes;
	THINK_NETCENTER_HANDLER handler;
	THINK_NETCENTER_HANDLE handle;
	int reof;
	int weof;
	struct __think_netcenter *netcenter;
	struct __think_netcenter_netlist *netlist;
};
typedef struct __think_netcenter_net THINK_NETCENTER_NET;

struct __think_netcenter_netlist{
	THINK_NETCENTER_NET *centernet;
	struct __think_netcenter_netlist *prior;
	struct __think_netcenter_netlist *next;
};
typedef struct __think_netcenter_netlist THINK_NETCENTER_NETLIST;

struct __think_netcenter{
	THINK_NETCENTER_NETLIST *netlist;

/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	int epfd;
#endif

/* kevent */
#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	int kq;
#endif
};
typedef struct __think_netcenter THINK_NETCENTER;

int think_netcenter_send(THINK_NETCENTER_NET *centernet,const void *buf,unsigned int len);
int think_netcenter_recv(THINK_NETCENTER_NET *centernet,void *buf,unsigned int siz);
int think_netcenter_peek(THINK_NETCENTER_NET *centernet,void *buf,unsigned int siz);

int think_netcenter_recvto(THINK_NETCENTER_NET *centernet);
int think_netcenter_sendfrom(THINK_NETCENTER_NET *centernet);

THINK_NETCENTER *think_netcenter_new();
int think_netcenter_idle(THINK_NETCENTER *netcenter,int timeout);
void think_netcenter_free(THINK_NETCENTER *netcenter);

THINK_NETCENTER_NET *think_netcenter_register(THINK_NETCENTER *netcenter,THINK_NET *net,THINK_NETCENTER_HANDLER handler,THINK_NETCENTER_HANDLE handle);
int think_netcenter_unregister(THINK_NETCENTER *netcenter,THINK_NETCENTER_NET *centernet);
int think_netcenter_shutdown(THINK_NETCENTER *netcenter,THINK_NETCENTER_NET *centernet);
THINK_NETCENTER_NETLIST *think_netcenter_find(THINK_NETCENTER *netcenter,THINK_NETCENTER_NET *centernet);
int think_netcenter_clean(THINK_NETCENTER *netcenter);

int think_netcenter_ismsgok(THINK_NETCENTER_NET *centernet);
int think_netcenter_recvmsg(THINK_NETCENTER_NET *centernet,void *buf,unsigned int siz);
int think_netcenter_sendmsg(THINK_NETCENTER_NET *centernet,const void *buf,unsigned int len);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_NETCENTER_H__ */
