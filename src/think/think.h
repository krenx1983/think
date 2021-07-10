/*
 * think.h: Think Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_H__
#define __THINK_H__

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* think_os */

/* Softwares */

#ifdef _AIX	/* IBM AIX OS */
#define __THINK_AIX__
#endif

#ifdef __hpux	/* HP HP-UX OS */
#define __THINK_HPUX__
#endif

#ifdef __SVR4	/* Sun Solaris OS */
#define __THINK_SOLARIS__
#endif

#ifdef __FreeBSD__	/* Berkeley FreeBSD OS */
#define __THINK_FREEBSD__
#endif

#ifdef __linux	/* GNU Linux OS */
#define __THINK_LINUX__
#endif

#ifdef __APPLE__	/* Apple Mac OS X */
#define __THINK_APPLE__
#endif

#ifdef _WIN32	/* Microsoft Windows OS */
#define __THINK_WINDOWS__
#endif

/* Hardwares */
#ifdef __sparc	/* Sun Sparc Machine */
#define __THINK_SPARC__
#endif

#ifdef __ppc__ /* IBM PowerPC */
#define __THINK_POWERPC__
#endif

#if defined(__THINK_AIX__) || defined(__THINK_HPUX__) || defined(__THINK_SOLARIS__) || defined(__THINK_FREEBSD__) || defined(__THINK_LINUX__) || defined(__THINK_APPLE__)
#define __THINK_UNIX__
#endif

#if defined(__THINK_WINDOWS__)	/* windows */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#define WIN32_LEAN_AND_MEAN	/* for socket api */
#include <windows.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__THINK_UNIX__)	/* unix */

/* hp-ux */
#ifdef __THINK_HPUX__
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#else	/* Unknown OS */
#error Operation System not supported!
#endif	/* __THINK_WINDOWS__ */


/* data type */
#ifdef __THINK_WINDOWS__
typedef __int16 THINK_SHORT;
typedef unsigned __int16 THINK_USHORT;
typedef __int32 THINK_INT;
typedef unsigned __int32 THINK_UINT;
typedef __int64 THINK_LONG;
typedef unsigned __int64 THINK_ULONG;
#else
typedef int16_t THINK_SHORT;
typedef uint16_t THINK_USHORT;
typedef int32_t THINK_INT;
typedef uint32_t THINK_UINT;
typedef int64_t THINK_LONG;
typedef uint64_t THINK_ULONG;
#endif	/* __THINK_WINDOWS__ */
typedef char THINK_CHAR;
typedef unsigned char THINK_UCHAR;
typedef float THINK_FLOAT;
typedef double THINK_DOUBLE;


/* byte order */

#if defined(__THINK_SPARC__) || defined(__THINK_AIX__) || defined(__THINK_HPUX__) || defined(__THINK_POWERPC__)
#define __THINK_BIG_ENDIAN__
#else
#define __THINK_LITTLE_ENDIAN__
#endif

extern THINK_ULONG l1;
extern THINK_ULONG l2;
extern THINK_ULONG l3;
extern THINK_ULONG l4;
extern THINK_ULONG l5;
extern THINK_ULONG l6;
extern THINK_ULONG l7;
extern THINK_ULONG l8;

#ifdef __THINK_LITTLE_ENDIAN__
#define think_htol2(s) (s)
#define think_htol4(i) (i)
#define think_htol8(l) (l)
#define think_ltoh2(s) (s)
#define think_ltoh4(i) (i)
#define think_ltoh8(l) (l)
#else
#define think_htol2(s) ((((s)&0x00FFU)<<8)+(((s)&0xFF00U)>>8))
#define think_htol4(i) ((((i)&0x000000FFU)<<24)+(((i)&0xFF000000U)>>24)\
	+(((i)&0x0000FF00U)<<8)+(((i)&0x00FF0000U)>>8))
#define think_htol8(l) ((((l)&l1)<<56)+(((l)&l8)>>56)\
	+(((l)&l2)<<40)+(((l)&l7)>>40)\
	+(((l)&l3)<<24)+(((l)&l6)>>24)\
	+(((l)&l4)<<8)+(((l)&l5)>>8))

#define think_ltoh2(s) ((((s)&0x00FFU)<<8)+(((s)&0xFF00U)>>8))
#define think_ltoh4(i) ((((i)&0x000000FFU)<<24)+(((i)&0xFF000000U)>>24)\
	+(((i)&0x0000FF00U)<<8)+(((i)&0x00FF0000U)>>8))
#define think_ltoh8(l) ((((l)&l1)<<56)+(((l)&l8)>>56)\
	+(((l)&l2)<<40)+(((l)&l7)>>40)\
	+(((l)&l3)<<24)+(((l)&l6)>>24)\
	+(((l)&l4)<<8)+(((l)&l5)>>8))
#endif /* __THINK_LITTLE_ENDIAN__ */

#ifdef __THINK_BIG_ENDIAN__
#define think_htob2(s) (s)
#define think_htob4(i) (i)
#define think_htob8(l) (l)
#define think_btoh2(s) (s)
#define think_btoh4(i) (i)
#define think_btoh8(l) (l)
#else
#define think_htob2(s) ((((s)&0x00FFU)<<8)+(((s)&0xFF00U)>>8))
#define think_htob4(i) ((((i)&0x000000FFU)<<24)+(((i)&0xFF000000U)>>24)\
	+(((i)&0x0000FF00U)<<8)+(((i)&0x00FF0000U)>>8))
#define think_htob8(l) ((((l)&l1)<<56)+(((l)&l8)>>56)\
	+(((l)&l2)<<40)+(((l)&l7)>>40)\
	+(((l)&l3)<<24)+(((l)&l6)>>24)\
	+(((l)&l4)<<8)+(((l)&l5)>>8))

#define think_btoh2(s) ((((s)&0x00FFU)<<8)+(((s)&0xFF00U)>>8))
#define think_btoh4(i) ((((i)&0x000000FFU)<<24)+(((i)&0xFF000000U)>>24)\
	+(((i)&0x0000FF00U)<<8)+(((i)&0x00FF0000U)>>8))
#define think_btoh8(l) ((((l)&l1)<<56)+(((l)&l8)>>56)\
	+(((l)&l2)<<40)+(((l)&l7)>>40)\
	+(((l)&l3)<<24)+(((l)&l6)>>24)\
	+(((l)&l4)<<8)+(((l)&l5)>>8))
#endif /* __THINK_BIG_ENDIAN__ */


/* c */

#ifdef __THINK_WINDOWS__
#ifdef __THINK_UNIX2WINDOWS__	/* transplant codes from unix to windows */
#define __func__ "__func__"
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define bzero(address,size) memset(address,0x00,size)
extern int opterr,optind,optopt;
extern char *optarg;
int getopt(int argc,char *argv[],const char *ostr);
#define atoll atol
#endif	/* __THINK_UNIX2WINDOWS__ */
#endif	/* __THINK_WINDOWS__ */

#ifdef __THINK_UNIX__
#ifdef __THINK_WINDOWS2UNIX__	/* transplant codes from windows to unix */
#define _snprintf snprintf
#define _vsnprintf vsnprintf
#endif	/* __THINK_WINDOWS2UNIX__ */

#ifdef __THINK_HPUX__
#define atoll(s) strtoll(s,(char **)NULL,10)
#endif

#endif	/* __THINK_UNIX__ */


/* errno */

#ifdef __THINK_UNIX__
#define think_strerror strerror
#define think_errno errno
#define think_socketerrno errno
#else

#ifndef EWOULDBLOCK 
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

const char *think_strerror(int e);
#define think_errno GetLastError()
#define think_socketerrno WSAGetLastError()
#endif	/* __THINK_UNIX__ */
#include <errno.h>


/* file */

#define seek lseek

#ifdef __THINK_WINDOWS__

#define think_mkdir(path,flags) mkdir(path)

#ifdef __THINK_UNIX2WINDOWS__
#define R_OK		4		/* Test for read permission */
#define W_OK		2		/* Test for write permission */
#define X_OK		1		/* Test for execute (search) permission */
#define F_OK		0		/* Test for existence of file */

#define O_RDONLY        _O_RDONLY
#define O_WRONLY        _O_WRONLY
#define O_RDWR          _O_RDWR
#define O_APPEND        _O_APPEND
#define O_CREAT         _O_CREAT
#define O_TRUNC         _O_TRUNC
#define O_EXCL          _O_EXCL
#define O_BINARY        _O_BINARY

#define open _open
#define read _read
#define write _write
#define lseek _lseek
#define close _close
#define access _access

#endif	/* __THINK_UNIX2WINDOWS__ */

#endif	/* __THINK_WINDOWS__ */

#ifdef __THINK_UNIX__

#ifdef __THINK_WINDOWS2UNIX__
#define _O_RDONLY       O_RDONLY
#define _O_WRONLY       O_WRONLY
#define _O_RDWR         O_RDWR
#define _O_APPEND       O_APPEND
#define _O_CREAT        O_CREAT
#define _O_TRUNC        O_TRUNC
#define _O_EXCL         O_EXCL
#define _O_BINARY       0

#define _open open
#define _read read
#define _write write
#define _lseek lseek
#define _close close
#define _access access

#endif	/* __THINK_WINDOWS2UNIX__ */

#define think_mkdir mkdir

#endif	/* __THINK_UNIX__ */


#ifdef __THINK_WINDOWS__
#define think_os_sleep(t) Sleep(t*1000)	/* second */
#define think_os_msleep Sleep			/* millisecond */
int think_os_usleep(unsigned int t);	/* microsecond */
int think_os_nsleep(unsigned int t);	/* nanosecond */
#else
#define think_os_sleep sleep			/* second */
int think_os_msleep(unsigned int t);	/* millisecond */
#define think_os_usleep usleep			/* microsecond */
int think_os_nsleep(unsigned int t);	/* nanosecond */
#endif


/* net */

#ifdef __THINK_WINDOWS__
#define socketclose closesocket
#define socklen_t int
#else
#define socketclose close
#endif

#ifndef INADDR_NONE
#define INADDR_NONE -1
#endif


/* think_error */

#define THINK_ERRNO_GL_FIELD_NOT_EXIST		1
#define THINK_ERRNO_GL_ATTR_NOT_EXIST		2

#define THINK_ERROR_ERRMSG_SIZE	1024
extern unsigned int __think_errno__;
extern char __think_errmsg__[THINK_ERROR_ERRMSG_SIZE];

void think_error(unsigned int e,const char *fmt,...);
void think_showerror(void);

#define think_errorerror() think_error(__think_errno__,"[%s]:%s",__func__,__think_errmsg__)


/* think_utility */

int think_bin_to_hex(char *dest,const char *src,int len);
int think_bin_to_bcd(char *dest,const char *src,int len);
int think_hex_to_bin(char *dest,const char *src,int len);
int think_bcd_to_bin(char *dest,const char *src,int len);
int think_hextobin(char *dest,const char *src);
int think_bcdtobin(char *dest,const char *src);
int think_bintohex(char *dest,char src);
int think_bintobcd(char *dest,char src);
int think_inttohex(int bin);
int think_inttobcd(int bin);
int think_hextoint(int hex);
int think_bcdtoint(int hex);
int think_printbin(const char *buf,const int len,int linesize);

/* string */

#define THINK_STR_CHAR		'C'		/* 1 bytes */
#define THINK_STR_SHORT		'S'		/* 2 bytes */
#define THINK_STR_INT		'I'		/* 4 bytes */
#define THINK_STR_LONG		'L'		/* 8 bytes */
#define THINK_STR_FLOAT		'F'		/* 4 bytes */
#define THINK_STR_DOUBLE	'D'		/* 8 bytes */

#define THINK_STR_NOT_NULL		0x80000000	/* not empty on str or 0 on numaric */
#define THINK_STR_NOT_STRING	0x40000000	/* not string */
#define THINK_STR_NOT_TRIM		0x20000000	/* not trim */

char *think_strtrim(char *str);
char *think_strinsert(char *srcstr,int pos,char *insstr);
char *think_chrinsert(char *srcstr,int pos,char insstr);
char *think_strreplace(char *srcstr,char *pattern,char *repstr);
char *think_chrreplace(char *srcstr,char pattern,char repstr);
char *think_strdelete(char *srcstr,const char *pattern);
char *think_chrdelete(char *srcstr,char delchr);
int think_strfind(const char *str,const char *pattern,int index);
int think_chrfind(const char *str,char chr,int index);
int think_chrstat(const char *str,char ch);
int think_strstat(const char *str,const char *pattern);

int think_strgetfield(const char *str,char sepchr,int index,char type,void *value,unsigned int size,int flags);

int think_bitset(char *bitmap,int bit);
int think_bittest(const char *bitmap,int bit);
int think_bitclear(char *bitmap,int bit);

#define THINK_OPT_CHAR		'C'		/* 1 bytes */
#define THINK_OPT_SHORT		'S'		/* 2 bytes */
#define THINK_OPT_INT		'I'		/* 4 bytes */
#define THINK_OPT_LONG		'L'		/* 8 bytes */
#define THINK_OPT_FLOAT		'F'		/* 4 bytes */
#define THINK_OPT_DOUBLE	'D'		/* 8 bytes */

#define THINK_OPT_ARG			0x80000000
#define THINK_OPT_NOT_IGNORE	0x40000000
#define THINK_OPT_NOT_NULL		0x20000000
#define THINK_OPT_NOT_STRING	0x10000000
#define THINK_OPT_NOT_TRIM		0x08000000

struct __think_optarray {
	char opt;
	char type;
	void *value;
	unsigned int size;
	int flags;
	int *exists;			/* 0:not exists,1:exists */
};
typedef struct __think_optarray THINK_OPTARRAY;
int think_getopt(int argc,char *argv[],char opt,char type,void *value,unsigned int size,int flags);
int think_getopts(int argc,char *argv[],THINK_OPTARRAY *optarray,unsigned int optcount);

#define THINK_ENV_CHAR		'C'		/* 1 bytes */
#define THINK_ENV_SHORT		'S'		/* 2 bytes */
#define THINK_ENV_INT		'I'		/* 4 bytes */
#define THINK_ENV_LONG		'L'		/* 8 bytes */
#define THINK_ENV_FLOAT		'F'		/* 4 bytes */
#define THINK_ENV_DOUBLE	'D'		/* 8 bytes */

#define THINK_ENV_NOT_IGNORE	0x80000000
#define THINK_ENV_NOT_NULL		0x40000000
#define THINK_ENV_NOT_STRING	0x20000000
#define THINK_ENV_NOT_TRIM		0x10000000

struct __think_envarray {
	const char *name;
	char type;
	void *value;
	unsigned int size;
	int flags;
	int *exists;
};
typedef struct __think_envarray THINK_ENVARRAY;
int think_getenv(const char *name,char type,void *value,int size,int flags);
int think_getenvs(THINK_ENVARRAY *envarray,unsigned int envcount);


/* think_file */

int think_automkdir(const char *path,int flags);


/* think_net */

#define THINK_NET_WAIT	0x80000000

struct __think_net {
	int sockfd;
	char ip[54];
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


/* think_netcenter */

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


/* think_timer */

typedef struct {
	unsigned int timeout_sec;	/* second */
	unsigned int timeout_msec;	/* millisecond */
	unsigned int lasttime_sec;	/* second */
	unsigned int lasttime_msec;	/* millisecond */
} THINK_TIMER;

THINK_TIMER *think_timernew(unsigned int timeout_msec);
void think_timerfree(THINK_TIMER *timer);
int think_timerrefresh(THINK_TIMER *timer);
void think_timerreset(THINK_TIMER *timer);


/* think_timercenter */

/* handle */
typedef void *THINK_TIMERCENTER_HANDLE;

/* handler */
typedef void (*THINK_TIMERCENTER_HANDLER)(THINK_TIMERCENTER_HANDLE);

/* timer */
struct __think_timercenter_timer {
	THINK_TIMER *timer;
	THINK_TIMERCENTER_HANDLER handler;
	THINK_TIMERCENTER_HANDLE handle;
};
typedef struct __think_timercenter_timer THINK_TIMERCENTER_TIMER;

/* timerlist */
struct __think_timercenter_timerlist {
	THINK_TIMERCENTER_TIMER *centertimer;
	struct __think_timercenter_timerlist *prior;
	struct __think_timercenter_timerlist *next;
};
typedef struct __think_timercenter_timerlist THINK_TIMERCENTER_TIMERLIST;

/* timercenter */
struct __think_timercenter {
	THINK_TIMERCENTER_TIMERLIST *timerlist;
};
typedef struct __think_timercenter THINK_TIMERCENTER;

/* new */
THINK_TIMERCENTER *think_timercenter_new(void);

/* free */
void think_timercenter_free(THINK_TIMERCENTER *timercenter);

/* register */
THINK_TIMERCENTER_TIMER *think_timercenter_register(THINK_TIMERCENTER *timercenter,THINK_TIMER *timer,THINK_TIMERCENTER_HANDLER handler,THINK_TIMERCENTER_HANDLE handle);

/* unregister */
int think_timercenter_unregister(THINK_TIMERCENTER *timercenter,THINK_TIMERCENTER_TIMER *centertimer);

/* find */
THINK_TIMERCENTER_TIMERLIST *think_timercenter_find(THINK_TIMERCENTER *timercenter,THINK_TIMERCENTER_TIMER *centertimer);

/* clean */
void think_timercenter_clean(THINK_TIMERCENTER *timercenter);

/* free */
void think_timercenter_free(THINK_TIMERCENTER *timercenter);

/* idle */
int think_timercenter_idle(THINK_TIMERCENTER *timercenter);


/* think_cfg */

#define THINK_CFG_FIELD_SIZE	128	/* field size */
#define THINK_CFG_NAME_SIZE	128	/* name size */
#define THINK_CFG_VALUE_SIZE	1024	/* value size */
#define THINK_CFG_FILE_SIZE	256	/* file size */

struct __think_cfg_cfglist {
	char field[THINK_CFG_FIELD_SIZE];	/* field */
	char name[THINK_CFG_NAME_SIZE];		/* name */
	char value[THINK_CFG_VALUE_SIZE];	/* value */
	char file[THINK_CFG_FILE_SIZE];		/* file */
	unsigned int lineno;
	struct __think_cfg_cfglist *prior;
	struct __think_cfg_cfglist *next;
};			/* cfg list */
typedef struct __think_cfg_cfglist THINK_CFG_CFGLIST;

struct __think_cfg{
	unsigned int cfgcount;		/* cfg count */
	THINK_CFG_CFGLIST *cfglist; 	/* cfg list */
};
typedef struct __think_cfg THINK_CFG;

#define THINK_CFG_CHAR			'C'		/* 1 bytes */
#define THINK_CFG_SHORT			'S'		/* 2 bytes */
#define THINK_CFG_INT			'I'		/* 4 bytes */
#define THINK_CFG_LONG			'L'		/* 8 bytes */
#define THINK_CFG_FLOAT			'F'		/* 4 bytes */
#define THINK_CFG_DOUBLE		'D'		/* 8 bytes */

#define THINK_CFG_NOT_IGNORE	0x80000000		/* not ignore */
#define THINK_CFG_NOT_NULL		0x40000000		/* not null */
#define THINK_CFG_NOT_STRING	0x20000000		/* not string */

struct __think_cfgarray {
	const char *domain;
	const char *name;
	char type;
	void *value;
	unsigned int size;
	int flags;
	int *exists;
};		/* cfg array */
typedef struct __think_cfgarray THINK_CFGARRAY;

/* load cfg */
THINK_CFG *think_loadcfg(const char *file,const char *path);

/* get cfg */
int think_getcfg(THINK_CFG *cfg,const char *field,const char *name,char type,void *value,unsigned int size,int flags);

/* show cfg */
int think_showcfg(THINK_CFG *cfg);

/* free cfg */
int think_freecfg(THINK_CFG *cfg);

/* get cfgs */
int think_getcfgs(THINK_CFG *cfg,THINK_CFGARRAY *cfgarray,unsigned int cfgcount);


/* think_conf */

/* field type */
#define THINK_CONF_CHAR		'C'		/* 1 bytes */
#define THINK_CONF_SHORT	'S'		/* 2 bytes */
#define THINK_CONF_INT		'I'		/* 4 bytes */
#define THINK_CONF_LONG		'L'		/* 8 bytes */
#define THINK_CONF_FLOAT	'F'		/* 4 bytes */
#define THINK_CONF_DOUBLE	'D'		/* 8 bytes */

#define THINK_CONF_NOT_NULL		0x80000000		/* field value not empty or 0 */
#define THINK_CONF_NOT_STRING	0x40000000		/* not string */

#define THINK_CONF_VALUE_SIZE	1024
#define THINK_CONF_FILE_SIZE	256

struct __think_conf_vlist {
	char value[THINK_CONF_VALUE_SIZE];	/* column value */
	struct __think_conf_vlist *prior;
	struct __think_conf_vlist *next;
};	/* column list */
typedef struct __think_conf_vlist THINK_CONF_VLIST;

struct __think_conf_hlist {
	struct __think_conf_vlist *vlist;		/* column list */
	struct __think_conf_hlist *prior;
	struct __think_conf_hlist *next;
};	/* line list */
typedef struct __think_conf_hlist THINK_CONF_HLIST;

struct __think_conf {
	char d;					/* column delimiter */
	unsigned int hcount;			/* line count */
	unsigned int vcount;			/* column count */
	char file[THINK_CONF_FILE_SIZE];	/* file */
	THINK_CONF_HLIST *hlist;		/* line list */
};
typedef struct __think_conf THINK_CONF;

THINK_CONF *think_loadconf(const char *file,char d);
int think_getconf(THINK_CONF *conf,int h,int v,char type,void *value,unsigned int size,int flags);               
int think_showconf(THINK_CONF *conf);
int think_freeconf(THINK_CONF *conf);


/* think_db */

/* open flags */
#define THINK_DB_CREAT 		0x80000000		/* creat */
#define THINK_DB_EXCL 		0x40000000		/* excl */
#define THINK_DB_TRUNC 		0x20000000		/* trunc */
#define THINK_DB_RDONLY 	0x10000000		/* read only */
#define THINK_DB_WRONLY 	0x08000000		/* write only */
#define THINK_DB_RDWR 		0x04000000		/* read & write */
#define THINK_DB_APPEND 	0x02000000		/* append */
#define THINK_DB_SYNC		0x01000000		/* sync */

/* seek */
#define THINK_DB_SEEK_SET	0		/* from first record */
#define THINK_DB_SEEK_CUR	1		/* from current record */
#define THINK_DB_SEEK_END	2		/* from last record */

/* field type */
#define THINK_DB_CHAR		'C'		/* 1 bytes */
#define THINK_DB_SHORT		'S'		/* 2 bytes */
#define THINK_DB_INT		'I'		/* 4 bytes */
#define THINK_DB_LONG		'L'		/* 8 bytes */
#define THINK_DB_FLOAT		'F'		/* 4 bytes */
#define THINK_DB_DOUBLE		'D'		/* 8 bytes */

/* flags */
#define THINK_DB_NOT_NULL	0x80000000		/* not null */
#define THINK_DB_NOT_STRING	0x40000000		/* not string */
#define THINK_DB_NOT_TRIM	0x20000000		/* not trim */

#define THINK_DB_EOF		-1		/* eof */

/* db field */
struct __think_db_field {
	char name[256];				/* name */
	char type;					/* type */
	THINK_UINT length;		/* length */
	THINK_UINT flags;			/* flags */
};
typedef struct __think_db_field THINK_DB_FIELD;

/* db handle */
struct __think_db {
	int fd;							/* db fd */
	unsigned char type;				/* db type */
	unsigned char version;			/* db version */
	int flags;						/* open flags */
	unsigned char field_count;		/* field count */
	THINK_DB_FIELD *field_array;	/* field array */
	char created_time[20];			/* created time */
	char last_modified_time[20];	/* last modified time */
	THINK_UINT record_count;		/* record count */
	unsigned char transaction_flag;	/* transaction flag */
	THINK_UINT option_length;		/* option length */
	char *option_buffer;			/* option buffer */

	THINK_UINT head_length;		/* head length */
	char opflag;					/* operation flag */
	int cursor;						/* cursor */
	char *record_buffer;			/* record buffer */
	unsigned int record_length;		/* record length */
	unsigned int *offset_array;		/* field offset array */
};
typedef struct __think_db THINK_DB;

/* think_db_open */
THINK_DB *think_db_open(const char *pathname,int flags,THINK_DB_FIELD *fieldarray,unsigned int fieldcount);
THINK_DB *think_db_openx(const char *pathname,int flags,const char *conffile);

/* think_db_fetch */
int think_db_fetch(THINK_DB *db);

/* think_db_getfield */
int think_db_getfield(THINK_DB *db,unsigned int no,void *value,unsigned int size,int flags);

/* think_db_getfieldbyname */
int think_db_getfieldbyname(THINK_DB *db,const char *name,void *value,unsigned int size,int flags);

/* think_db_new */
void think_db_new(THINK_DB *db);

/* think_db_putfield */
int think_db_putfield(THINK_DB *db,unsigned int no,const void *value,unsigned int length,int flags);

/* think_db_putfieldbyname */
int think_db_putfieldbyname(THINK_DB *db,const char *name,const void *value,unsigned int length,int flags);

/* think_db_insert */
int think_db_insert(THINK_DB *db);

/* think_db_delete */
int think_db_delete(THINK_DB *db);

/* think_db_seek */
int think_db_seek(THINK_DB *db,int offset,int whence);

/* think_db_sync */
int think_db_sync(THINK_DB *db);

/* think_db_eof */
int think_db_eof(THINK_DB *db);

/* think_db_close */
int think_db_close(THINK_DB *db);

/* think_db_getflags */
int think_db_getfieldflags(const char *strflags);


/* think_gl */

#define THINK_GL_CHAR		'C'		/* 1 bytes */
#define THINK_GL_SHORT		'S'		/* 2 bytes */
#define THINK_GL_INT		'I'		/* 4 bytes */
#define THINK_GL_LONG		'L'		/* 8 bytes */
#define THINK_GL_FLOAT		'F'		/* 4 bytes */
#define THINK_GL_DOUBLE		'D'		/* 8 bytes */

/* flags */
#define THINK_GL_BINARY		0x80000000
#define THINK_GL_UNSIGNED	0x40000000
#define THINK_GL_RECORD		0x20000000	/* data is a single record */
#define THINK_GL_RECORDSET	0x10000000

/* opflags */
#define THINK_GL_IGNORE		0x00008000
#define THINK_GL_NOT_STRING	0x00004000
#define THINK_GL_NOT_TRIM	0x00002000
#define THINK_GL_NOT_NULL	0x00001000	/* not empty on char,not zero on numeric */
#define THINK_GL_FIXLEN		0x00000800	/* length must be a fix */
#define THINK_GL_DEBUG		0x00000400	/* debug mode(send attr info) */
#define THINK_GL_UPDATE		0x00000200	/* update field */

/* attribute no */
#define THINK_GL_NAME		0
#define THINK_GL_TYPE		1
#define THINK_GL_MAXLEN		2		/* integer length on numeric */
#define THINK_GL_FLAGS		3
#define THINK_GL_DESC		4

/* rule */
typedef struct {
	THINK_USHORT no;	/* no */
	char name[32];	/* name */
	char type;		/* type */
	THINK_UINT maxlen;	/* maximal length */
	THINK_INT flags;	/* flags */
	char desc[128];	/* desc */
} THINK_GL_RULE;

/* field */
typedef struct {
	unsigned short no;	/* no */
	void *data;		/* data */
	unsigned int size;	/* size */
	unsigned int *len;	/* length */
	int *exist;	/* exist */
	int opflags;	/* opflags */
} THINK_GL_FIELD;

/* field rule */
typedef struct {
	THINK_USHORT no;	/* no */
	char name[32];	/* name */
	char type;		/* type */
	THINK_UINT maxlen;	/* maximal length */
	THINK_INT flags;	/* flags */
	char desc[128];	/* desc */
	void *data;		/* data */
	unsigned int size;	/* size */
	unsigned int *len;	/* length */
	int *exist;	/* exist */
	int opflags;	/* opflags */
} THINK_GL_FIELDRULE;

/* think_gl_getflags */
int think_gl_getflags(const char *strflags);

/* think_gl_find */
const char *think_gl_find(const char *msgbuf,unsigned int msglen,unsigned short no);

/* think_gl_attr_find */
const char *think_gl_attr_find(const char *attrbuf,unsigned short attrlen,unsigned char attrno);

/* think_gl_attr_get */
int think_gl_attr_get(const char *attrbuf,unsigned short attrlen,unsigned char attrno,void *data,unsigned char size);

/* think_gl_attr_put */
int think_gl_attr_put(char *attrbuf,unsigned short attrsiz,unsigned char attrno,const void *data,unsigned char len);

/* think_gl_record_get */
int think_gl_record_get(const char *msgbuf,unsigned int msglen,char *recbuf,unsigned int recsiz);

/* think_gl_record_put */
int think_gl_record_put(char *msgbuf,unsigned int msgsiz,const char *recbuf,unsigned int reclen);

/* think_gl_record_stat */
int think_gl_record_stat(const char *msgbuf,unsigned int msglen);

/* think_gl_record_fetch */
int think_gl_record_fetch(const char *msgbuf,unsigned int msglen,unsigned int recno,char *recbuf,unsigned int recsiz);

/* think_gl_get */
int think_gl_get(const char *msgbuf,unsigned int msglen,unsigned short no,char *name,char type,unsigned int maxlen,int flags,char *desc,void *data,unsigned int size,int opflags);

/* think_gl_put */
int think_gl_put(char *msgbuf,unsigned int msgsiz,unsigned short no,const char *name,char type,unsigned int maxlen,int flags,const char *desc,const void *data,unsigned int len,int opflags);

/* think_gl_del */
int think_gl_del(char *msgbuf,unsigned int msglen,unsigned short no);

/* think_gl_gets */
int think_gl_gets(const char *msgbuf,unsigned int msglen,const THINK_GL_FIELDRULE *fieldrulearray,unsigned int fieldrulecount,int opflags);

/* think_gl_puts */
int think_gl_puts(char *msgbuf,unsigned int msgsiz,const THINK_GL_FIELDRULE *fieldrulearray,unsigned int fieldrulecount,int opflags);

/* think_gl_autoget */
int think_gl_autoget(const char *msgbuf,unsigned int msglen,unsigned short no,void *data,unsigned int size,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags);

/* think_gl_autoput */
int think_gl_autoput(char *msgbuf,unsigned int msgsiz,unsigned short no,const void *data,unsigned int len,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags);

/* think_gl_autogets */
int think_gl_autogets(const char *msgbuf,unsigned int msglen,const THINK_GL_FIELD *fieldarray,unsigned int fieldcount,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags);

/* think_gl_autoputs */
int think_gl_autoputs(char *msgbuf,unsigned int msgsiz,const THINK_GL_FIELD *fieldarray,unsigned int fieldcount,const THINK_GL_RULE *rulearray,unsigned int rulecount,int opflags);

/* think_gl_loadrules */
int think_gl_loadrules(const char *file,THINK_GL_RULE **rulearray);


/* think_log */

#define THINK_LOG_DEBUG		1	/* debug */
#define THINK_LOG_NORMAL	2	/* normal(default) */
#define THINK_LOG_INFO		3	/* info */
#define THINK_LOG_KEY		4	/* key */
#define THINK_LOG_WARN		5	/* warn */
#define THINK_LOG_ERROR		6	/* error */
#define THINK_LOG_FATAL		7	/* fatal */

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


/* think_shm */

#define THINK_SHM_CREAT		0x80000000
#define THINK_SHM_EXCL		0x40000000
#define THINK_SHM_TRUNC		0x20000000
#define THINK_SHM_RDONLY	0x10000000
#define THINK_SHM_RDWR		0x08000000
#define THINK_SHM_RUSR		0x00000100		/*0400(r)*/
#define THINK_SHM_WUSR		0x00000080		/*0200(w)*/
#define THINK_SHM_XUSR		0x00000040		/*0100(x)*/
#define THINK_SHM_RGRP		0x00000020		/*0040(r)*/
#define THINK_SHM_WGRP		0x00000010		/*0020(w)*/
#define THINK_SHM_XGRP		0x00000008		/*0010(x)*/
#define THINK_SHM_ROTH		0x00000004		/*0004(r)*/
#define THINK_SHM_WOTH		0x00000002		/*0002(w)*/
#define THINK_SHM_XOTH		0x00000001		/*0001(x)*/

#ifndef SHM_FAILED
#define SHM_FAILED (void *)-1L
#endif

struct __think_shm{
#ifdef __THINK_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int shmid;
#endif
	void *addr;
};
typedef struct __think_shm THINK_SHM;
THINK_SHM *think_shmopen(const char *name,int size,int flags);
int think_shmclose(THINK_SHM *shm);


/* think_sem.h */

#define THINK_SEM_CREAT		0x80000000
#define THINK_SEM_TRUNC		0x40000000
#define THINK_SEM_EXCL		0x20000000
#define THINK_SEM_UNDO		0x10000000
#define THINK_SEM_RUSR		0x00000100		/*0400(r)*/
#define THINK_SEM_WUSR		0x00000080		/*0200(w)*/
#define THINK_SEM_XUSR		0x00000040		/*0100(x)*/
#define THINK_SEM_RGRP		0x00000020		/*0040(r)*/
#define THINK_SEM_WGRP		0x00000010		/*0020(w)*/
#define THINK_SEM_XGRP		0x00000008		/*0010(x)*/
#define THINK_SEM_ROTH		0x00000004		/*0004(r)*/
#define THINK_SEM_WOTH		0x00000002		/*0002(w)*/
#define THINK_SEM_XOTH		0x00000001		/*0001(x)*/

struct __think_sem{
#ifdef __THINK_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int semid;
#endif
	int flags;
};
typedef struct __think_sem THINK_SEM;

#ifdef __THINK_UNIX__ /* UNIX */
union __think_semun{
	int val;
	struct semid_ds *buf;
	ushort *array;
#ifdef __THINK_LINUX__ /* linux */
	struct seminfo *__buf;
#endif /* __THINK_LINUX__ */
};
typedef union __think_semun THINK_SEMUN;
#endif /* __THINK_UNIX__ */

THINK_SEM *think_semopen(const char *name,int flags);
int think_semget(THINK_SEM *sem,int timeout);
int think_semput(THINK_SEM *sem);
int think_semclose(THINK_SEM *sem);


/* think_mutex */

#define THINK_MUTEX_CREAT		0x80000000
#define THINK_MUTEX_TRUNC		0x40000000
#define THINK_MUTEX_EXCL		0x20000000
#define THINK_MUTEX_RUSR		0x00000100		/*0400(r)*/
#define THINK_MUTEX_WUSR		0x00000080		/*0200(w)*/
#define THINK_MUTEX_XUSR		0x00000040		/*0100(x)*/
#define THINK_MUTEX_RGRP		0x00000020		/*0040(r)*/
#define THINK_MUTEX_WGRP		0x00000010		/*0020(w)*/
#define THINK_MUTEX_XGRP		0x00000008		/*0010(x)*/
#define THINK_MUTEX_ROTH		0x00000004		/*0004(r)*/
#define THINK_MUTEX_WOTH		0x00000002		/*0002(w)*/
#define THINK_MUTEX_XOTH		0x00000001		/*0001(x)*/

struct __think_mutex{
#ifdef __THINK_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int semid;
#endif
	int flags;
};
typedef struct __think_mutex THINK_MUTEX;

THINK_MUTEX *think_mutexopen(const char *name,int flags);
int think_mutexlock(THINK_MUTEX *mutex,int timeout);
int think_mutexunlock(THINK_MUTEX *mutex);
int think_mutexclose(THINK_MUTEX *mutex);


/* think_shmq */

/* open mode */
#define THINK_SHMQ_CREAT		0x80000000
#define THINK_SHMQ_EXCL			0x40000000
#define THINK_SHMQ_TRUNC		0x20000000
#define THINK_SHMQ_RDONLY		0x10000000
#define THINK_SHMQ_RUSR			0x00000100		/*0400(r)*/
#define THINK_SHMQ_WUSR			0x00000080		/*0200(w)*/
#define THINK_SHMQ_XUSR			0x00000040		/*0100(x)*/
#define THINK_SHMQ_RGRP			0x00000020		/*0040(r)*/
#define THINK_SHMQ_WGRP			0x00000010		/*0020(w)*/
#define THINK_SHMQ_XGRP			0x00000008		/*0010(x)*/
#define THINK_SHMQ_ROTH			0x00000004		/*0004(r)*/
#define THINK_SHMQ_WOTH			0x00000002		/*0002(w)*/
#define THINK_SHMQ_XOTH			0x00000001		/*0001(x)*/

typedef struct {
	int size;
	int totalsize;
	int freesize;
	int head;
	int tail;
	int msgnum;
}THINK_SHMQHEAD;

typedef struct {
	THINK_SHM *shm;
	THINK_SEM *sem;
	THINK_MUTEX *mutex;
	int flags;
	THINK_SHMQHEAD *head;
	void *data;
}THINK_SHMQ;

THINK_SHMQ *think_shmqopen(const char *name,int size,int flags);
int think_shmqget(THINK_SHMQ *shmq,void *buf,int size,int timeout);
int think_shmqput(THINK_SHMQ *shmq,const void *buf,int len,int timeout);
int think_shmqclose(THINK_SHMQ *shmq);


/* think_thread */

struct __think_thread{
#ifdef __THINK_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	pthread_t thread;
#endif
};
typedef struct __think_thread THINK_THREAD;
	
#ifdef __THINK_WINDOWS__ /* Windows */
typedef int (WINAPI *THINK_OS_THREADROUTINE)(void *);
int WINAPI think_os_threadroutine(void *);
#else /* UNIX */
typedef void *(*THINK_OS_THREADROUTINE)(void *);
void *think_os_threadroutine(void *);
#endif

/* think thread routine and arg */
typedef void (*THINK_THREADROUTINE)(void *);
struct __think_threadarg{
	THINK_THREADROUTINE routine;
	void *arg;
};
typedef struct __think_threadarg THINK_THREADARG;
	
THINK_THREAD *think_threadcreate(THINK_THREADROUTINE routine,void *arg);
int think_threadexit(int exitcode);
int think_threadcancel(THINK_THREAD *thread);
int think_threadjoin(THINK_THREAD *thread,int *exitcode,int timeout);


/* think_threadmutex */

struct __think_threadmutex{
#ifdef __THINK_WINDOWS__ /* Windows */
	CRITICAL_SECTION cs;
#else /* UNIX */
	pthread_mutex_t *threadmutex;
#endif
};
typedef struct __think_threadmutex THINK_THREADMUTEX;


THINK_THREADMUTEX *think_threadmutex_open();
int think_threadmutex_lock(THINK_THREADMUTEX *threadmutex,int timeout);
int think_threadmutex_unlock(THINK_THREADMUTEX *threadmutex);
int think_threadmutex_close(THINK_THREADMUTEX *threadmutex);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_H__ */
