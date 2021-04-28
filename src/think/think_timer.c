#include "think_error.h"
#include "think_utility.h"
#include "think_timer.h"

THINK_TIMER *think_timernew(unsigned int timeout_msec)
{
	THINK_TIMER *timer;
	unsigned int timenow_sec,timenow_msec;
#ifdef __THINK_UNIX__
	struct timeval t;
#else
	unsigned int t;
#endif

	if((timer=malloc(sizeof(THINK_TIMER)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		return NULL;
	}
	memset(timer,0x00,sizeof(THINK_TIMER));
	timer->timeout_sec=timeout_msec/1000;
	timer->timeout_msec=timeout_msec%1000;
#ifdef __THINK_WINDOWS__
	t=GetTickCount();
	timenow_sec=t/1000;
	timenow_msec=t%1000;
#else
	gettimeofday(&t,NULL);
	timenow_sec=t.tv_sec;
	timenow_msec=t.tv_usec/1000;
#endif
	timer->lasttime_sec=timenow_sec;
	timer->lasttime_msec=timenow_msec;

	return timer;
}

void think_timerfree(THINK_TIMER *timer)
{
	free(timer);
}

int think_timerrefresh(THINK_TIMER *timer)
{
	int timenow_sec,timenow_msec;
	int lasttime_sec,lasttime_msec;
	int timeout_sec,timeout_msec;
#ifdef __THINK_UNIX__
	struct timeval t;
#else
	unsigned int t;
#endif

#ifdef __THINK_WINDOWS__
	t=GetTickCount();
	timenow_sec=t/1000;
	timenow_msec=t%1000;
#else
	gettimeofday(&t,NULL);
	timenow_sec=t.tv_sec;
	timenow_msec=t.tv_usec/1000;
#endif

	if(timenow_sec<timer->lasttime_sec || (timenow_sec==timer->lasttime_sec && timenow_msec<timer->lasttime_msec)){
		timer->lasttime_sec=timenow_sec;
		timer->lasttime_msec=timenow_msec;
		return 0;
	}

	lasttime_sec=timer->lasttime_sec;
	lasttime_msec=timer->lasttime_msec;
	timeout_sec=timer->timeout_sec;
	timeout_msec=timer->timeout_msec;

	if((timenow_sec-lasttime_sec)*1000+(timenow_msec-lasttime_msec)>timeout_sec*1000+timeout_msec){
		timer->lasttime_sec=timenow_sec;
		timer->lasttime_msec=timenow_msec;
		return 1;
	}

	return 0;
}
void think_timerreset(THINK_TIMER *timer)
{
	unsigned int timenow_sec,timenow_msec;
#ifdef __THINK_UNIX__
	struct timeval t;
#else
	unsigned int t;
#endif
	
#ifdef __THINK_WINDOWS__
	t=GetTickCount();
	timenow_sec=t/1000;
	timenow_msec=t%1000;
#else
	gettimeofday(&t,NULL);
	timenow_sec=t.tv_sec;
	timenow_msec=t.tv_usec/1000;
#endif
	timer->lasttime_sec=timenow_sec;
	timer->lasttime_msec=timenow_msec;
}
