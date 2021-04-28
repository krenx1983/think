#include "think_error.h"
#include "think_utility.h"
#include "think_timercenter.h"

THINK_TIMERCENTER *think_timercenter_new(void)
{
	THINK_TIMERCENTER *timercenter;

	timercenter=malloc(sizeof(THINK_TIMERCENTER));
	memset(timercenter,0x00,sizeof(THINK_TIMERCENTER));

	return timercenter;
}

THINK_TIMERCENTER_TIMER *think_timercenter_register(THINK_TIMERCENTER *timercenter,THINK_TIMER *timer,THINK_TIMERCENTER_HANDLER handler,THINK_TIMERCENTER_HANDLE handle)
{
	THINK_TIMERCENTER_TIMER *centertimer;
	THINK_TIMERCENTER_TIMERLIST *head,*tail;
	THINK_TIMERCENTER_TIMERLIST *p;

	centertimer=malloc(sizeof(THINK_TIMERCENTER_TIMER));
	memset(centertimer,0x00,sizeof(THINK_TIMERCENTER_TIMER));
	centertimer->timer=timer;
	centertimer->handler=handler;
	centertimer->handle=handle;

	p=malloc(sizeof(THINK_TIMERCENTER_TIMERLIST));
	memset(p,0x00,sizeof(THINK_TIMERCENTER_TIMERLIST));
	p->centertimer=centertimer;
	if(timercenter->timerlist==NULL){
		p->prior=p;
		p->next=p;
		timercenter->timerlist=p;
	}else{
		head=timercenter->timerlist;
		tail=timercenter->timerlist->prior;
		p->prior=tail;
		tail->next=p;
		p->next=head;
		head->prior=p;
	}

	return centertimer;
}

int think_timercenter_unregister(THINK_TIMERCENTER *timercenter,THINK_TIMERCENTER_TIMER *centertimer)
{
	THINK_TIMERCENTER_TIMERLIST *p;
	
	if((p=think_timercenter_find(timercenter,centertimer))==NULL){
		think_error(0,"[%s]:timer not exists.",__func__);
		return -1;
	}
	free(p->centertimer);
	p->centertimer=NULL;
	
	return 0;
}

THINK_TIMERCENTER_TIMERLIST *think_timercenter_find(THINK_TIMERCENTER *timercenter,THINK_TIMERCENTER_TIMER *centertimer)
{
	THINK_TIMERCENTER_TIMERLIST *p;
	
	if(timercenter->timerlist==NULL)
		return NULL;
	p=timercenter->timerlist;
	do{
		if(p->centertimer==centertimer)
			return p;
		p=p->next;
	}while(p!=timercenter->timerlist);
	
	return NULL;
}
void think_timercenter_free(THINK_TIMERCENTER *timercenter)
{
	THINK_TIMERCENTER_TIMERLIST *p;

	if(timercenter->timerlist==NULL){
		free(timercenter);
		return;
	}

	p=timercenter->timerlist;
	do{
		p->centertimer=NULL;
		p=p->next;
	}while(p!=timercenter->timerlist);
	think_timercenter_clean(timercenter);
	free(timercenter);
}
void think_timercenter_clean(THINK_TIMERCENTER *timercenter)
{
	THINK_TIMERCENTER_TIMERLIST *next,*prior;
	THINK_TIMERCENTER_TIMERLIST *p;
	
	if(timercenter->timerlist==NULL)
		return;

	/* clean */
	p=timercenter->timerlist;
	do{
		if(p==timercenter->timerlist && p->centertimer==NULL){
			if(p==p->next){
				timercenter->timerlist=NULL;
				free(p);
				break;
			}else{
				timercenter->timerlist=p->next;
				next=p->next;
				prior=p->prior;
				prior->next=next;
				next->prior=prior;				
				free(p);
				p=timercenter->timerlist;
				continue;
			}
		}
		if(p->centertimer!=NULL){
			p=p->next;
			if(p==timercenter->timerlist)
				break;
			continue;
		}
		next=p->next;
		prior=p->prior;
		prior->next=next;
		next->prior=prior;
		free(p);
		p=next;
		if(p==timercenter->timerlist)
			break;
	}while(1);
}

int think_timercenter_idle(THINK_TIMERCENTER *timercenter)
{
	THINK_TIMERCENTER_TIMERLIST *p;
	THINK_TIMERCENTER_TIMER *centertimer;
	int n;
	
	think_timercenter_clean(timercenter);
	if(timercenter->timerlist==NULL)
		return 0;

	p=timercenter->timerlist;
	n=0;
	do{
		centertimer=p->centertimer;
		if(!centertimer){
			p=p->next;
			continue;
		}
		if(think_timerrefresh(centertimer->timer)){
			centertimer->handler(centertimer->handle);
			n++;
		}
		p=p->next;
	}while(p!=timercenter->timerlist);

	return n;
}
