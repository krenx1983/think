/*
 * think_timercenter.h: Think Timer Center Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(luojian1983@sohu.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __THINK_TIMERCENTER_H__
#define __THINK_TIMERCENTER_H__

#include "think_os.h"
#include "think_timer.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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

/* C++ */
#ifdef __cplusplus
}
#endif

#endif /* __THINK_TIMERCENTER_H__ */
