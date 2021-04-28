/*
 * think_error.h: Think Error Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_ERROR_H__
#define __THINK_ERROR_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define THINK_ERRNO_GL_FIELD_NOT_EXIST		1
#define THINK_ERRNO_GL_ATTR_NOT_EXIST		2

#define THINK_ERROR_ERRMSG_SIZE	1024
extern int __think_errno__;
extern char __think_errmsg__[THINK_ERROR_ERRMSG_SIZE];

void think_error(unsigned int e,const char *fmt,...);
void think_showerror(void);

#define think_errorerror() think_error(__think_errno__,"[%s]:%s",__func__,__think_errmsg__)

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_ERROR_H__ */
