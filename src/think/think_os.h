/*
 * think_os.h: Think OS Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_OS_H__
#define __THINK_OS_H__

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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

#pragma warning (disable:4786) 
#pragma warning (disable:4018)
#pragma warning (disable:4761)
#pragma warning (disable:4244)

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
#include <netdb.h>

#include <pthread.h>

#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#else	/* Unsupported OS */
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


/* C++ */
#ifdef __cplusplus
}
#endif

#endif /* __THINK_OS_H__ */
