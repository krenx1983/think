#include "think_error.h"
#include "think_netcenter.h"

/* poll */
#ifdef __THINK_UNIX__
#include <poll.h>
#endif

/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
#include <sys/epoll.h>
#endif

/* kevent */
#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
#include <sys/event.h>
#endif

int think_netcenter_recvto(THINK_NETCENTER_NET *centernet)
{
	THINK_NETCENTER_BUFLIST *p,*head,*tail;
	char *buf;
	int n;

	if(centernet->reof || centernet->weof){
		think_error(0,"[%s]:connection disconnected.",__func__);
		return -1;
	}
	if(centernet->rlist && centernet->rlist->prior->len<THINK_NETCENTER_BUFSIZE){
		p=centernet->rlist->prior;	/* Add to tail */
		if(p->pos+p->len == THINK_NETCENTER_BUFSIZE){	/* Reset pos to reuse the free space */
			memmove(p->buf,p->buf+p->pos,p->len);
			p->pos=0;
		}
		if((n=think_netrecv(centernet->net,p->buf+(p->pos+p->len),THINK_NETCENTER_BUFSIZE-(p->pos+p->len),0))<=0){
			think_errorerror();
			centernet->reof=1;
			return -1;
		}
		p->len+=n;
	}else{	/* New Buffer */
		buf=malloc(THINK_NETCENTER_BUFSIZE);
		if((n=think_netrecv(centernet->net,buf,THINK_NETCENTER_BUFSIZE,0))<=0){
			think_errorerror();
			free(buf);
			centernet->reof=1;
			return -1;
		}
		p=malloc(sizeof(THINK_NETCENTER_BUFLIST));
		memset(p,0x00,sizeof(THINK_NETCENTER_BUFLIST));
		p->buf=buf;
		p->pos=0;
		p->len=n;
		p->prior=NULL;
		p->next=NULL;
		if(centernet->rlist==NULL){
			p->prior=p;
			p->next=p;
			centernet->rlist=p;
		}else{
			head=centernet->rlist;
			tail=centernet->rlist->prior;
			p->prior=tail;
			p->next=head;
			tail->next=p;
			head->prior=p;
		}
	}
	centernet->nrbytes+=n;

	return n;
}

int think_netcenter_send(THINK_NETCENTER_NET *centernet,const void *buf,unsigned int len)
{
	THINK_NETCENTER_BUFLIST *p,*head,*tail;
	int pos,n;
	char *buffer;

	if(centernet->reof || centernet->weof){
		think_error(0,"[%s]:connection disconnected.",__func__);
		return -1;
	}

	/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	if(centernet->nwbytes == 0){
		struct epoll_event ev;

		bzero(&ev,sizeof(ev));
		ev.events=EPOLLOUT|EPOLLIN;
		ev.data.ptr = centernet->netlist;
		epoll_ctl(centernet->netcenter->epfd,EPOLL_CTL_MOD,centernet->net->sockfd,&ev);
	}
#endif

	/* kevent */
#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	if(centernet->nwbytes == 0){
		struct kevent changelist[1];

		EV_SET(changelist,centernet->net->sockfd,EVFILT_WRITE,EV_ADD,0,0,centernet->netlist);
		kevent(centernet->netcenter->kq,changelist,1,NULL,0,NULL);
	}
#endif

	for(pos=0;pos<len;pos+=n,centernet->nwbytes+=n){
		if(centernet->wlist && centernet->wlist->prior->len<THINK_NETCENTER_BUFSIZE){
			p=centernet->wlist->prior;	/* Add to tail */
			if(p->pos+p->len == THINK_NETCENTER_BUFSIZE){	/* Reset pos to reuse the free space */
				memmove(p->buf,p->buf+p->pos,p->len);
				p->pos=0;
			}
			n=len-pos>THINK_NETCENTER_BUFSIZE-(p->pos+p->len)?THINK_NETCENTER_BUFSIZE-(p->pos+p->len):len-pos;
			memcpy(p->buf+p->len,(const char *)buf+pos,n);
			p->len+=n;
		}else{
			n=len-pos>THINK_NETCENTER_BUFSIZE?THINK_NETCENTER_BUFSIZE:len-pos;
			buffer=malloc(THINK_NETCENTER_BUFSIZE);
			memcpy(buffer,(const char *)buf+pos,n);
			p=malloc(sizeof(THINK_NETCENTER_BUFLIST));
			memset(p,0x00,sizeof(THINK_NETCENTER_BUFLIST));
			p->buf=(char *)buffer;
			p->pos=0;
			p->len=n;
			p->prior=NULL;
			p->next=NULL;
			if(centernet->wlist==NULL){
				p->next=p;
				p->prior=p;
				centernet->wlist=p;
			}else{
				head=centernet->wlist;
				tail=centernet->wlist->prior;
				p->prior=tail;
				p->next=head;
				tail->next=p;
				head->prior=p;
			}
		}
	}

	return 0;
}

int think_netcenter_recv(THINK_NETCENTER_NET *centernet,void *buf,unsigned int siz)
{
	THINK_NETCENTER_BUFLIST *p,*prior,*next;
	int n,len;

	if(centernet->weof){
		think_error(0,"[%s]:connection disconnected.",__func__);
		return -1;
	}
	if(centernet->nrbytes==0 && centernet->reof){
		think_error(0,"[%s]:connection disconnected.",__func__);
		return -1;
	}
	for(len=0;len<siz;len+=n,centernet->nrbytes-=n){
		p=centernet->rlist;
		if(!p || p->len==0)
			break;
		n=siz-len>p->len?p->len:siz-len;
		memcpy((char *)buf+len,p->buf+p->pos,n);
		p->len-=n;
		p->pos+=n;
		if(p->len==0){
			if(p==p->next){
				p->pos=0;	// Keep the only buffer. Just reset the pos.
			}else{
				prior=p->prior;
				next=p->next;
				prior->next=next;
				next->prior=prior;
				centernet->rlist=next;
				free(p->buf);
				free(p);
			}
		}
	}

	return len;
}

int think_netcenter_sendfrom(THINK_NETCENTER_NET *centernet)
{
	THINK_NETCENTER_BUFLIST *p,*prior,*next;
	int n;

	p=centernet->wlist;
	if((n=think_netsend(centernet->net,p->buf+p->pos,p->len,0))<0)
		return -1;
	if(n==0)
		return 0;
	centernet->nwbytes-=n;
	p->len-=n;
	p->pos+=n;
	if(p->len>0)
		return n;

	/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	if(centernet->nwbytes == 0){
		struct epoll_event ev;

		bzero(&ev,sizeof(ev));
		ev.events=EPOLLIN;
		ev.data.ptr = centernet->netlist;
		epoll_ctl(centernet->netcenter->epfd,EPOLL_CTL_MOD,centernet->net->sockfd,&ev);
	}
#endif

	/* kevent */
#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	if(centernet->nwbytes == 0){
		struct kevent changelist[1];

		EV_SET(changelist,centernet->net->sockfd,EVFILT_WRITE,EV_DELETE,0,0,centernet->netlist);
		kevent(centernet->netcenter->kq,changelist,1,NULL,0,NULL);
	}
#endif

	if(p==p->next){
		p->pos=0;	// Keep the only buffer. Just reset the pos.
	}else{
		prior=p->prior;
		next=p->next;
		prior->next=next;
		next->prior=prior;
		centernet->wlist=next;
		free(p->buf);
		free(p);
	}

	return n;
}

int think_netcenter_peek(THINK_NETCENTER_NET *centernet,void *buf,unsigned int siz)
{
	THINK_NETCENTER_BUFLIST *p;
	int n,len;

	if(centernet->weof){
		think_error(0,"[%s]:connection disconnected.",__func__);
		return -1;
	}
	if(centernet->nrbytes==0 && centernet->reof){
		think_error(0,"[%s]:connection disconnected.",__func__);
		return -1;
	}
	len=0;
	p=centernet->rlist;
	do{
		if(!p || p->len==0)
			break;
		n=siz-len>p->len?p->len:siz-len;
		memcpy((char *)buf+len,p->buf+p->pos,n);
		len+=n;
		if(len==siz)
			break;
		p=p->next;
	}while(p!=centernet->rlist);
	
	return len;
}

THINK_NETCENTER *think_netcenter_new()
{
	THINK_NETCENTER *netcenter;

	netcenter=malloc(sizeof(THINK_NETCENTER));
	memset(netcenter,0x00,sizeof(THINK_NETCENTER));

/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	netcenter->epfd=epoll_create(THINK_NETCENTER_MAX_CONNECTIONS);
#endif

/* kevent */
#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	netcenter->kq=kqueue();
#endif

	return netcenter;
}
void think_netcenter_free(THINK_NETCENTER *netcenter)
{
	THINK_NETCENTER_NETLIST *p;

	if(netcenter->netlist==NULL){
		free(netcenter);
		return;
	}
	p=netcenter->netlist;
	do{
		p->centernet=NULL;
		p=p->next;
	}while(p!=netcenter->netlist);
	think_netcenter_clean(netcenter);
	free(netcenter);
}
THINK_NETCENTER_NET *think_netcenter_register(THINK_NETCENTER *netcenter,THINK_NET *net,THINK_NETCENTER_HANDLER handler,THINK_NETCENTER_HANDLE handle)
{
	THINK_NETCENTER_NETLIST *head,*tail;
	THINK_NETCENTER_NETLIST *p;
	THINK_NETCENTER_NET *centernet;

	/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	struct epoll_event ev;
#endif

	/* kevent */
#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	struct kevent changelist[1];
#endif

	/* Set NonBlock */
#ifdef __THINK_WINDOWS__
	unsigned long ul=1;
	ioctlsocket(net->sockfd,FIONBIO,&ul);
#else
	fcntl(net->sockfd,F_SETFL,O_NONBLOCK);
#endif

	centernet=malloc(sizeof(THINK_NETCENTER_NET));
	memset(centernet,0x00,sizeof(THINK_NETCENTER_NET));
	centernet->netcenter=netcenter;
	centernet->net=net;
	centernet->handler=handler;
	centernet->handle=handle;

	p=malloc(sizeof(THINK_NETCENTER_NETLIST));
	memset(p,0x00,sizeof(THINK_NETCENTER_NETLIST));
	p->centernet=centernet;
	if(netcenter->netlist==NULL){
		p->prior=p;
		p->next=p;
		netcenter->netlist=p;
	}else{
		head=netcenter->netlist;
		tail=netcenter->netlist->prior;
		p->prior=tail;
		tail->next=p;
		p->next=head;
		head->prior=p;
	}
	centernet->netlist=p;

	/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	bzero(&ev,sizeof(ev));
	ev.events=EPOLLIN;
	ev.data.ptr = centernet->netlist;
	epoll_ctl(netcenter->epfd,EPOLL_CTL_ADD,net->sockfd,&ev);
#endif

#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	EV_SET(changelist,centernet->net->sockfd,EVFILT_READ,EV_ADD,0,0,centernet->netlist);
	kevent(centernet->netcenter->kq,changelist,1,NULL,0,NULL);
#endif

	return centernet;
}
int think_netcenter_shutdown(THINK_NETCENTER *netcenter,THINK_NETCENTER_NET *centernet)
{
	THINK_NET *net;

	net=centernet->net;
	if(centernet->nwbytes==0){
		think_netcenter_unregister(netcenter,centernet);
		think_netclose(net);
	}else{
		centernet->weof=1;
	}

	return 0;
}
int think_netcenter_unregister(THINK_NETCENTER *netcenter,THINK_NETCENTER_NET *centernet)
{
	THINK_NETCENTER_NETLIST *p;
	THINK_NETCENTER_BUFLIST *np;

	/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	struct epoll_event ev;
#endif

	/* kevent */
#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	struct kevent changelist[1];
#endif

	p=centernet->netlist;

	/* epoll */
#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	bzero(&ev,sizeof(ev));
	ev.data.ptr = centernet->netlist;
	epoll_ctl(netcenter->epfd,EPOLL_CTL_DEL,centernet->net->sockfd,&ev);
#endif

#if (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)
	EV_SET(changelist,centernet->net->sockfd,EVFILT_READ,EV_DELETE,0,0,centernet->netlist);
	kevent(centernet->netcenter->kq,changelist,1,NULL,0,NULL);
	EV_SET(changelist,centernet->net->sockfd,EVFILT_WRITE,EV_DELETE,0,0,centernet->netlist);
	kevent(centernet->netcenter->kq,changelist,1,NULL,0,NULL);
#endif

	/* free rlist */
	while(p->centernet->rlist){
		np=p->centernet->rlist;
		if(np==np->next){
			free(np->buf);
			free(np);
			break;
		}
		np->next->prior=np->prior;
		np->prior->next=np->next;
		p->centernet->rlist=np->next;
		free(np->buf);
		free(np);
	}

	/* free wlist */
	while(p->centernet->wlist){
		np=p->centernet->wlist;
		if(np==np->next){
			free(np->buf);
			free(np);
			break;
		}
		np->next->prior=np->prior;
		np->prior->next=np->next;
		p->centernet->wlist=np->next;
		free(np->buf);
		free(np);
	}
	free(centernet);
	p->centernet=NULL;

	return 0;
}
THINK_NETCENTER_NETLIST *think_netcenter_find(THINK_NETCENTER *netcenter,THINK_NETCENTER_NET *centernet)
{
	THINK_NETCENTER_NETLIST *p;
	
	if(netcenter->netlist==NULL)
		return NULL;
	p=netcenter->netlist;
	do{
		if(p->centernet==centernet)
			return p;
		p=p->next;
	}while(p!=netcenter->netlist);

	return NULL;
}
int think_netcenter_clean(THINK_NETCENTER *netcenter)
{
	THINK_NETCENTER_NETLIST *next,*prior;
	THINK_NETCENTER_NETLIST *p;

	if(netcenter->netlist==NULL)
		return 0;
	/* clean */
	p=netcenter->netlist;
	do{
		if(p==netcenter->netlist && p->centernet==NULL){
			if(p==p->next){
				netcenter->netlist=NULL;
				free(p);
				return 0;
			}else{
				netcenter->netlist=p->next;
				next=p->next;
				prior=p->prior;
				prior->next=next;
				next->prior=prior;				
				free(p);
				p=netcenter->netlist;
				continue;
			}
		}
		if(p->centernet!=NULL){
			p=p->next;
			if(p==netcenter->netlist)
				break;
			continue;
		}
		next=p->next;
		prior=p->prior;
		prior->next=next;
		next->prior=prior;
		free(p);
		p=next;
		if(p==netcenter->netlist)
			break;
	}while(1);

	return 0;
}

int think_netcenter_sendmsg(THINK_NETCENTER_NET *centernet,const void *buf,unsigned int len)
{
	THINK_UINT t;

	t=len;
	t=think_htob4(t);
	think_netcenter_send(centernet,&t,4);
	think_netcenter_send(centernet,buf,len);

	return 0;
}
int think_netcenter_recvmsg(THINK_NETCENTER_NET *centernet,void *buf,unsigned int siz)
{
	THINK_INT len;

	think_netcenter_recv(centernet,&len,4);
	len=think_btoh4(len);
	if(len>siz){
		think_error(0,"[%s]:msg too long![siz=%d][len=%d]",__func__,siz,len);
		return -1;
	}
	think_netcenter_recv(centernet,buf,len);

	return len;
}
int think_netcenter_ismsgok(THINK_NETCENTER_NET *centernet)
{
	THINK_INT len,n;

	if((n=think_netcenter_peek(centernet,&len,4))<=0)
		return n;
	if(n!=4){
		if(centernet->reof){
			think_error(0,"[%s]:message not ok,but connection is disconnected.",__func__);
			return -1;
		}
		return 0;
	}
	len=think_btoh4(len);
	if(centernet->nrbytes>=len+4)
		return 1;
	if(centernet->reof){
		think_error(0,"[%s]:message not ok,but connection is disconnected.",__func__);
		return -1;
	}

	return 0;
}

#if defined(__THINK_LINUX__) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)

/* epoll */
int think_netcenter_idle(THINK_NETCENTER *netcenter,int timeout)
{
	struct epoll_event events[THINK_NETCENTER_MAX_CONNECTIONS];
	THINK_NET *net;
	THINK_NETCENTER_NET *centernet;
	THINK_NETCENTER_NETLIST *p;
	int n,i;

	think_netcenter_clean(netcenter);
	if(netcenter->netlist==NULL)
		return 0;
	if((n=epoll_wait(netcenter->epfd,events,THINK_NETCENTER_MAX_CONNECTIONS,timeout))<0){
		if(think_socketerrno==EINTR)
			return 0;
		think_error(0,"[%s]:epoll error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return -1;
	}
	if(n==0)
		return 0;
	for(i=0;i<n;i++){
		p=(THINK_NETCENTER_NETLIST *)events[i].data.ptr;
		centernet=p->centernet;

		if(centernet==NULL)
			continue;

		net=centernet->net;

		/* recv */
		if(events[i].events & EPOLLIN){
			centernet->handler(centernet->handle);
		}

		if(p->centernet==NULL)
			continue;

		/* send */
		if(events[i].events & EPOLLOUT){
			if(think_netcenter_sendfrom(centernet)<0){
				if(centernet->weof){
					think_netclose(net);
					think_netcenter_unregister(netcenter,centernet);
				}
			}else{
				if(centernet->nwbytes==0){
					if(centernet->weof){
						think_netclose(net);
						think_netcenter_unregister(netcenter,centernet);
					}
				}
			}
		}
	}

	return n;
}

#elif (defined(__THINK_APPLE__) || defined(__THINK_FREEBSD__)) && !defined(THINK_NETCENTER_SELECT) && !defined(THINK_NETCENTER_POLL)

/* kevent */
int think_netcenter_idle(THINK_NETCENTER *netcenter,int timeout)
{
	struct kevent events[THINK_NETCENTER_MAX_CONNECTIONS];
	THINK_NET *net;
	THINK_NETCENTER_NET *centernet;
	THINK_NETCENTER_NETLIST *p;
	struct timespec t,*pt;
	int n,i;

	think_netcenter_clean(netcenter);
	if(netcenter->netlist==NULL)
		return 0;
	if(timeout<0)
		pt=NULL;
	else{
		pt=&t;
		t.tv_sec=timeout/1000;
		t.tv_nsec=timeout%1000*1000000;
	}
	if((n=kevent(netcenter->kq,NULL,0,events,THINK_NETCENTER_MAX_CONNECTIONS,pt))<0){
		if(think_socketerrno==EINTR)
			return 0;
		think_error(0,"[%s]:kevent error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return -1;
	}
	if(n==0)
		return 0;
	for(i=0;i<n;i++){
		p=(THINK_NETCENTER_NETLIST *)events[i].udata;
		centernet=p->centernet;

		if(centernet==NULL)
			continue;

		net=centernet->net;

		/* recv */
		if(events[i].filter == EVFILT_READ){
			centernet->handler(centernet->handle);
		}

		if(p->centernet==NULL)
			continue;

		/* send */
		if(events[i].filter == EVFILT_WRITE){
			if(think_netcenter_sendfrom(centernet)<0){
				if(centernet->weof){
					think_netclose(net);
					think_netcenter_unregister(netcenter,centernet);
				}
			}else{
				if(centernet->nwbytes==0){
					if(centernet->weof){
						think_netclose(net);
						think_netcenter_unregister(netcenter,centernet);
					}
				}
			}
		}
	}

	return n;
}

#elif defined(__THINK_WINDOWS__) || !defined(THINK_NETCENTER_POLL)

/* select */
int think_netcenter_idle(THINK_NETCENTER *netcenter,int timeout)
{
	fd_set fdrset,fdwset;
	int maxfd;
	THINK_NET *net;
	THINK_NETCENTER_NET *centernet;
	THINK_NETCENTER_NETLIST *p;
	struct timeval t,*pt;
	int n;

	think_netcenter_clean(netcenter);
	if(netcenter->netlist==NULL)
		return 0;
	FD_ZERO(&fdrset);
	FD_ZERO(&fdwset);
	maxfd=0;
	p=netcenter->netlist;
	do{
		centernet=p->centernet;
		net=centernet->net;
		if(!centernet->weof)
			FD_SET(net->sockfd,&fdrset);
		if(!centernet->reof)
			if(centernet->nwbytes!=0)
				FD_SET(net->sockfd,&fdwset);
		maxfd=net->sockfd>maxfd?net->sockfd:maxfd;
		p=p->next;
	}while(p!=netcenter->netlist);
	if(timeout<0)
		pt=NULL;
	else{
		pt=&t;
		t.tv_sec=timeout/1000;
		t.tv_usec=timeout%1000*1000;
	}
	if((n=select(maxfd+1,&fdrset,&fdwset,NULL,pt))<0){
		if(think_socketerrno==EINTR)
			return 0;
		think_error(0,"[%s]:select error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return -1;
	}
	if(n==0)
		return 0;
	p=netcenter->netlist;
	do{
		centernet=p->centernet;
		if(centernet==NULL){
			p=p->next;
			continue;
		}
		net=centernet->net;

		/* recv */
		if(FD_ISSET(net->sockfd,&fdrset))
			centernet->handler(centernet->handle);

		if(p->centernet==NULL){
			p=p->next;
			continue;
		}

		/* send */
		if(FD_ISSET(net->sockfd,&fdwset)){
			if(think_netcenter_sendfrom(centernet)<0){
				if(centernet->weof){
					think_netclose(net);
					think_netcenter_unregister(netcenter,centernet);
				}
			}else{
				if(centernet->nwbytes==0){
					if(centernet->weof){
						think_netclose(net);
						think_netcenter_unregister(netcenter,centernet);
					}
				}
			}
		}

		p=p->next;
	}while(p!=netcenter->netlist);

	return n;
}

#else

/* poll */
int think_netcenter_idle(THINK_NETCENTER *netcenter,int timeout)
{
	struct pollfd fdarray[THINK_NETCENTER_MAX_CONNECTIONS];
	int nfds;
	THINK_NET *net;
	THINK_NETCENTER_NET *centernet;
	THINK_NETCENTER_NETLIST *p;
	int n,i;

	think_netcenter_clean(netcenter);
	if(netcenter->netlist==NULL)
		return 0;

	nfds=0;
	p=netcenter->netlist;
	do{
		centernet=p->centernet;
		net=centernet->net;
		fdarray[nfds].fd=net->sockfd;
		if(!centernet->weof)
			fdarray[nfds].events=POLLIN;
		if(!centernet->reof)
			if(centernet->nwbytes!=0)
				fdarray[nfds].events|=POLLOUT;
		fdarray[nfds].revents=0;
		nfds++;
		if(nfds==THINK_NETCENTER_MAX_CONNECTIONS)
			break;
		p=p->next;
	}while(p!=netcenter->netlist);

	if((n=poll(fdarray,nfds,timeout))<0){
		if(think_socketerrno==EINTR)
			return 0;
		think_error(0,"[%s]:poll error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return -1;
	}
	if(n==0)
		return 0;

	p=netcenter->netlist;
	do{
		centernet=p->centernet;
		if(centernet==NULL){
			p=p->next;
			continue;
		}
		net=centernet->net;

		for(i=0;i<nfds;i++){
			if(fdarray[i].fd==net->sockfd)
				break;
		}
		if(i==nfds){
			p=p->next;
			continue;
		}

		/* recv */
		if(fdarray[i].revents&POLLIN)
			centernet->handler(centernet->handle);

		/* send */
		if(p->centernet==NULL){
			p=p->next;
			continue;
		}
		if(fdarray[i].revents&POLLOUT){
			if(think_netcenter_sendfrom(centernet)<0){
				if(centernet->weof){
					think_netclose(net);
					think_netcenter_unregister(netcenter,centernet);
				}
			}else{
				if(centernet->nwbytes==0){
					if(centernet->weof){
						think_netclose(net);
						think_netcenter_unregister(netcenter,centernet);
					}
				}
			}
		}

		p=p->next;
	}while(p!=netcenter->netlist);

	return n;
}

#endif
