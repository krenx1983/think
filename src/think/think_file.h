/*
 * think_file.h: Think File Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: krenx(krenx@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_FILE_H__
#define __THINK_FILE_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

int think_automkdir(const char *path,int flags);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __THINK_FILE_H__ */
