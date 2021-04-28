/*
 * think_timer.h: Think Timer Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_TIMER_H__
#define __THINK_TIMER_H__

#include "think_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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

/* C++ */
#ifdef __cplusplus
}
#endif

#endif /* __THINK_TIMER_H__ */
